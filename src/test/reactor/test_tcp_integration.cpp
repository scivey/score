#include <glog/logging.h>
#include <thread>
#include <gtest/gtest.h>
#include "aliens/FixedBuffer.h"
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/ReactorThread.h"
#include "aliens/reactor/SignalFd.h"
#include "aliens/reactor/TimerFd.h"
#include "aliens/reactor/EventFd.h"
#include "aliens/reactor/TimerSettings.h"
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/net/TCPAcceptSocket.h"
#include "aliens/net/TCPClient.h"
#include "aliens/net/TCPChannel.h"
#include "aliens/locks/ThreadBaton.h"
#include "aliens/io/NonOwnedBufferPtr.h"
#include "aliens/io/string_utils.h"
#include "aliens/async/ErrBack.h"
#include "aliens/async/VoidCallback.h"
#include "aliens/async/Callback.h"
#include "aliens/exceptions/macros.h"
#include "aliens/macros.h"
#include <signal.h>
#include <folly/Demangle.h>
#include <folly/Conv.h>
#include <glog/logging.h>

using aliens::io::NonOwnedBufferPtr;

using namespace std;
using namespace aliens::async;
using namespace aliens::net;
using namespace aliens::posix;
using namespace aliens::reactor;
using namespace aliens::locks;

namespace {

NonOwnedBufferPtr makeStupidBuffer(const std::string &msg) {
  string result = msg;
  result += "\r\n";
  auto copied = (char*) malloc(result.size() + 1);
  memcpy((void*) copied, (void*) result.data(), result.size() - 1);
  copied[result.size()] = '\0';
  return NonOwnedBufferPtr{copied, result.size() - 1};
}


std::string buffStr(void *buff, size_t buffLen) {
  auto data = (char*) buff;
  std::string asStr;
  asStr.reserve(buffLen);
  for (size_t i = 0; i < buffLen; i++) {
    asStr.push_back(data[i]);
  }
  return asStr;
}

std::string prettyBuff(void *buff, size_t buffLen) {
  auto data = (char*) buff;
  std::string pretty {"'"};
  for (size_t i = 0; i < buffLen; i++) {
    char c = data[i];
    if (c == '\r') {
      pretty.push_back('\\');
      pretty.push_back('r');
    } else if (c == '\n') {
      pretty.push_back('\\');
      pretty.push_back('n');
    } else {
      pretty.push_back(c);
    }
  }
  pretty.push_back('\'');
  return pretty;
}

class SomeRequestHandler : public TCPChannel::EventHandler {
 protected:
  std::string lastBuff_;
  bool isWritable_ {false};
 public:
  void onConnectSuccess() override {}
  void onConnectError(int err) override {
    ACHECK(false, strerror(err));
  }
  void onReadableStart() override {
    readSome();
  }
  void onReadableStop() override {
    LOG(INFO) << "server onReadableStop";
  }
  void getReadBuffer(void **buff, size_t *buffLen, size_t hint) override {
    void *buffPtr = malloc(hint);
    memset(buffPtr, 0, hint);
    *buff = buffPtr;
    *buffLen = hint;
  }
  void readBufferAvailable(void *buff, size_t buffLen) override {
    auto data = (const char*) buff;
    lastBuff_ = "";
    LOG(INFO) << "server got: " << prettyBuff(buff, buffLen);
    for (size_t i = 0; i <= buffLen; i++) {
      lastBuff_.push_back(data[i]);
    }
    if (isWritable_) {
      doEcho();
    }
  }
  void doEcho() {
    if (isWritable_ && !lastBuff_.empty()) {
      isWritable_ = false;
      auto buffPtr = makeStupidBuffer(lastBuff_);
      lastBuff_ = "";
      sendBuff(buffPtr, [](const ErrBack::except_option &err) {
        ACHECK(!err.hasValue());
      });
    }
  }
  void onWritable() override {
    isWritable_ = true;
    if (!lastBuff_.empty()) {
      doEcho();
    }
  }
  void onEOF() override {
    shutdown();
  }
};



class RequestHandlerFactory {
 public:
  virtual TCPChannel::EventHandler* getHandler() = 0;
  virtual ~RequestHandlerFactory() = default;
};

class AcceptHandler : public TCPAcceptSocket::EventHandler {
 protected:
  std::unique_ptr<RequestHandlerFactory> handlerFactory_ {nullptr};
 public:
  AcceptHandler(std::unique_ptr<RequestHandlerFactory> &&factory)
    : handlerFactory_(std::forward<std::unique_ptr<RequestHandlerFactory>>(factory)){}

  void onAcceptSuccess(int sfd, const char *hostName, const char *portNo) override {
    LOG(INFO) << "onAcceptSuccess : [" << sfd << "] : " << hostName << ":" << portNo;
    auto desc = FileDescriptor::fromIntExcept(sfd);
    desc.makeNonBlocking();
    SocketAddr localAddr("127.0.0.1", 9999);
    SocketAddr remoteAddr(hostName, portNo);
    TCPConnectionInfo connInfo;
    connInfo.localAddr = localAddr;
    connInfo.remoteAddr = remoteAddr;
    auto handler = handlerFactory_->getHandler();
    auto channel = TCPChannel::fromDescriptorPtr(std::move(desc),
      handlerFactory_->getHandler(), connInfo
    );
    getParent()->getReactor()->addTask(channel->getEpollTask());
  }
  void onAcceptError(int err) override {
    LOG(INFO) << "onAcceptError [" << err << "]";
  }
};

class SomeFactory: public RequestHandlerFactory {
 public:
  TCPChannel::EventHandler* getHandler() override {
    return new SomeRequestHandler;
  }
};

class ClientHandler : public TCPChannel::EventHandler {
 protected:
  std::string msg_ {"DEFAULT"};
  VoidCallback onFinished_;
  bool sent_ {false};
  bool gotResponse_ {false};
  std::vector<std::string> responses_;
 public:
  ClientHandler(const std::string& msg, VoidCallback&& cb)
    : msg_(msg), onFinished_(std::forward<VoidCallback>(cb)) {}

  vector<string> copyResponses() const {
    vector<string> copied;
    for (auto &item: responses_) {
      copied.push_back(item);
    }
    return copied;
  }

  void getReadBuffer(void **buff, size_t *buffLen, size_t hint) override {
    void *buffPtr = malloc(hint);
    memset(buffPtr, 0, hint);
    *buff = buffPtr;
    *buffLen = hint;
  }
  void readBufferAvailable(void *buff, size_t buffLen) override {
    auto asStr = buffStr(buff, buffLen);
    LOG(INFO) << "client got: " << prettyBuff(buff, buffLen);
    // auto charPtr = (char*) buff;

    // ostringstream oss;
    // for (size_t i = 0; i < buffLen; i++) {
    //   auto c = *charPtr;
    //   oss << c;
    //   ++charPtr;
    // }
    // auto asStr = oss.str();
    // LOG(INFO) << "client got: " << asStr;
    responses_.push_back(asStr);
  }
  void onReadableStart() override {
    LOG(INFO) << "Client onReadableStart.";
    if (sent_) {
      gotResponse_ = true;
    }
    readSome();
  }
  void onReadableStop() override {
    LOG(INFO) << "Client onReadableStop.";
    if (gotResponse_) {
      onFinished_();
      shutdown();
    }
  }
  void onConnectSuccess() override {}
  void onConnectError(int err) override {
    ACHECK(false, strerror(err));
  }
  void onWritable() override {
    if (!sent_) {
      sent_ = true;
      auto buffPtr = makeStupidBuffer(msg_);
      sendBuff(buffPtr, [](const ErrBack::except_option &ex) {
        ACHECK(!ex.hasValue());
      });
    }
  }
  void onEOF() override {
    LOG(INFO) << "onEOF.";
  }
};

}

static const short kPortNum = 9051;


TEST(TestTCPIntegration, Test1) {
  auto react = ReactorThread::createShared();
  EpollReactor::Options options;
  options.setWaitTimeout(std::chrono::milliseconds(10));
  react->start(options);
  ThreadBaton bat1;
  react->runInEventThread([&bat1, react]() {
    auto acceptor = new TCPAcceptSocket(TCPAcceptSocket::bindPort(
      kPortNum, new AcceptHandler(std::unique_ptr<SomeFactory>(new SomeFactory))
    ));
    acceptor->listen();
    react->addTask(acceptor->getEpollTask(), [&bat1]() {
      LOG(INFO) << "added acceptor.";
      bat1.post();
    });
  });
  bat1.wait();
  ThreadBaton finalBaton;
  ThreadBaton bat2;
  std::atomic<ClientHandler*> clientHandlerPtr {nullptr};
  react->runInEventThread([&bat2, react, &clientHandlerPtr, &finalBaton]() {
    SocketAddr addr {"127.0.0.1", kPortNum};
    auto handler = new ClientHandler("{A_MESSAGE}\r\n", [&finalBaton]() {
      finalBaton.post();
    });
    clientHandlerPtr.store(handler);
    auto client = TCPClient::connectPtr(
      react->getReactor(), handler, addr
    );
    react->addTask(client->getEpollTask(), [&bat2]() {
      LOG(INFO) << "added client.";
      bat2.post();
    });
  });
  bat2.wait();
  finalBaton.wait();
  react->runInEventThread([react]() {
    react->stop([](const ErrBack::except_option& err) {
      ACHECK(!err.hasValue());
    });
  });
  react->join();
  auto clientResponses = clientHandlerPtr.load()->copyResponses();
  EXPECT_EQ(1, clientResponses.size());
  auto response = aliens::io::trimAsciiWhitespace(
    clientResponses.at(0)
  );
  EXPECT_EQ("{A_MESSAGE}", response);
}
