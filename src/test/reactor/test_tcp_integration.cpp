#include <glog/logging.h>
#include <thread>
#include <gtest/gtest.h>
#include <stdexcept>
#include <exception>
#include "score/FixedBuffer.h"
#include "score/reactor/EpollReactor.h"
#include "score/reactor/ReactorThread.h"
#include "score/reactor/SignalFd.h"
#include "score/reactor/TimerFd.h"
#include "score/reactor/EventFd.h"
#include "score/reactor/TimerSettings.h"
#include "score/reactor/FdHandlerBase.h"
#include "score/net/TCPAcceptSocket.h"
#include "score/net/TCPClient.h"
#include "score/net/TCPChannel.h"
#include "score/locks/ThreadBaton.h"
#include "score/io/NonOwnedBufferPtr.h"
#include "score/io/string_utils.h"
#include "score/async/ErrBack.h"
#include "score/async/VoidCallback.h"
#include "score/async/Callback.h"
#include "score/exceptions/macros.h"
#include "score/macros.h"
#include <signal.h>
#include <folly/Demangle.h>
#include <folly/Conv.h>
#include <glog/logging.h>

using score::io::NonOwnedBufferPtr;

using namespace std;
using namespace score::async;
using namespace score::net;
using namespace score::posix;
using namespace score::reactor;
using namespace score::locks;

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
    SCHECK(false, strerror(err));
  }
  void onReadableStart() override {
    readSome();
  }
  void onReadableStop() override {
    VLOG(50) << "server onReadableStop";
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
    VLOG(50) << "server got: " << prettyBuff(buff, buffLen);
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
        SCHECK(!err.hasValue());
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
    auto desc = FileDescriptor::fromIntExcept(sfd);
    desc.makeNonBlocking();
    SocketAddr localAddr("127.0.0.1", 9999);
    SocketAddr remoteAddr(hostName, portNo);
    TCPConnectionInfo connInfo;
    connInfo.localAddr = localAddr;
    connInfo.remoteAddr = remoteAddr;
    auto handler = handlerFactory_->getHandler();
    auto channel = TCPChannel::Factory::createRaw(std::move(desc),
      handlerFactory_->getHandler(), connInfo
    );
    getParent()->getReactor()->addTask(channel->getEpollTask());
  }
  void onAcceptError(int err) override {
    std::ostringstream msg;
    msg << "accept error: " << err;
    throw std::runtime_error(msg.str());
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
    responses_.push_back(asStr);
  }
  void onReadableStart() override {
    if (sent_) {
      gotResponse_ = true;
    }
    readSome();
  }
  void onReadableStop() override {
    if (gotResponse_) {
      onFinished_();
      shutdown();
    }
  }
  void onConnectSuccess() override {}
  void onConnectError(int err) override {
    SCHECK(false, strerror(err));
  }
  void onWritable() override {
    if (!sent_) {
      sent_ = true;
      auto buffPtr = makeStupidBuffer(msg_);
      sendBuff(buffPtr, [](const ErrBack::except_option &ex) {
        SCHECK(!ex.hasValue());
      });
    }
  }
  void onEOF() override {}
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
      bat2.post();
    });
  });
  bat2.wait();
  finalBaton.wait();
  react->runInEventThread([react]() {
    react->stop([](const ErrBack::except_option& err) {
      SCHECK(!err.hasValue());
    });
  });
  react->join();
  auto clientResponses = clientHandlerPtr.load()->copyResponses();
  EXPECT_EQ(1, clientResponses.size());
  auto response = score::io::trimAsciiWhitespace(
    clientResponses.at(0)
  );
  EXPECT_EQ("{A_MESSAGE}", response);
}
