#include <glog/logging.h>
#include <thread>
#include <pthread.h>

#include "aliens/test_support/Noisy.h"

// #include <spdlog/spdlog.h>
#include "aliens/FixedBuffer.h"
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/ReactorThread.h"
#include "aliens/reactor/SignalFd.h"
#include "aliens/reactor/TimerFd.h"
#include "aliens/reactor/EventFd.h"
#include "aliens/reactor/TCPSocket.h"
#include "aliens/reactor/AcceptSocketTask.h"
#include "aliens/reactor/ClientSocketTask.h"
#include "aliens/reactor/ServerSocketTask.h"
#include "aliens/reactor/TimerSettings.h"
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/reactor/TCPAcceptSocket.h"
#include "aliens/reactor/TCPClient.h"
#include "aliens/reactor/TCPChannel.h"
#include "aliens/locks/ThreadBaton.h"
#include "aliens/io/NonOwnedBufferPtr.h"
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

template<typename T>
void sayType(const T& ref) {
  auto demangled = folly::demangle(typeid(T));
  auto asStr = folly::to<std::string>(demangled);
  LOG(INFO) << "[type: '" << demangled << "'] : '" << ref << "'";
}

using namespace std;
using namespace aliens::async;
using namespace aliens::reactor;
using namespace aliens::locks;



NonOwnedBufferPtr makeStupidBuffer(const std::string &msg) {
  string result = msg;
  result += "\r\n";
  auto copied = (char*) malloc(result.size() + 1);
  memcpy((void*) copied, (void*) result.data(), result.size() - 1);
  copied[result.size()] = '\0';
  return NonOwnedBufferPtr{copied, result.size() - 1};
}

class SomeRequestHandler : public TCPChannel::EventHandler {
 protected:
  std::string lastBuff_;
  bool isWritable_ {false};
 public:
  SomeRequestHandler() {
    LOG(INFO) << "SomeRequestHandler()";
  }
  void onConnectSuccess() override {
    LOG(INFO) << "onConnectSuccess.";
  }
  void onConnectError(int err) override {
    LOG(INFO) << "onConnectError : " << strerror(err);
  }
  void onReadableStart() override {
    LOG(INFO) << "SomeRequestHandler::onReadableStart()";
    readSome();
  }
  void onReadableStop() override {
    LOG(INFO) << "SomeRequestHandler::onReadableStop()";
  }
  void getReadBuffer(void **buff, size_t *buffLen, size_t hint) override {
    LOG(INFO) << "getReadBuffer : hint=" << hint;
    void *buffPtr = malloc(hint);
    memset(buffPtr, 0, hint);
    *buff = buffPtr;
    *buffLen = hint;
  }
  void readBufferAvailable(void *buff, size_t buffLen) override {
    LOG(INFO) << "readBufferAvailable : " << buffLen;
    auto data = (const char*) buff;
    LOG(INFO) << "\tread: '" << data << "'";
    lastBuff_ = "";
    for (size_t i = 0; i < buffLen; i++) {
      char c = data[i];
      lastBuff_.push_back(c);
    }
    LOG(INFO) << "getFdNo..";
    getParent()->getFdNo();
    LOG(INFO) << "/getFdNo..";

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
        LOG(INFO) << "sendBuff callback";
        if (err.hasValue()) {
          LOG(INFO) << "err : " << err.value().what();
        }
      });
    }
  }
  void onWritable() override {
    LOG(INFO) << "onWritable.";
    isWritable_ = true;
    if (!lastBuff_.empty()) {
      doEcho();
    }
  }
  void onEOF() override {
    LOG(INFO) << "onEOF.";
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
    // auto serverSock = new TCPServerSocket(TCPServerSocket::fromAccepted(
    //   std::move(desc), handler, localAddr, remoteAddr
    // ));
    LOG(INFO) << "made server socket..";
    LOG(INFO) << "\t\t server socket "
              << "{accept_fd=" << getParent()->getFdNo() << "}"
              << " {server_fd=" << channel->getFdNo() << "}";
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
  bool sent_ {false};
  bool gotResponse_ {false};
  std::string msg_ {"DEFAULT"};
 public:
  ClientHandler(){}
  ClientHandler(const std::string& msg): msg_(msg){}
  void getReadBuffer(void **buff, size_t *buffLen, size_t hint) override {
    LOG(INFO) << "client getReadBuffer : hint=" << hint;
    void *buffPtr = malloc(hint);
    memset(buffPtr, 0, hint);
    *buff = buffPtr;
    *buffLen = hint;
  }
  void readBufferAvailable(void *buff, size_t buffLen) override {
    LOG(INFO) << "client readBufferAvailable : " << buffLen;
    auto data = (const char*) buff;
    LOG(INFO) << "\tclient read: '" << data << "'";
    // shutdown();
  }
  void onReadableStart() override {
    LOG(INFO) << "onReadableStart.";
    if (sent_) {
      gotResponse_ = true;
    }
    readSome();
  }
  void onReadableStop() override {
    LOG(INFO) << "onReadableStop.";
    if (gotResponse_) {
      shutdown();
    }
  }
  void onConnectSuccess() override {
    LOG(INFO) << "CONNECTED.";
  }
  void onConnectError(int err) override {
    LOG(INFO) << "client err! " << strerror(err);
  }
  void onWritable() override {
    LOG(INFO) << "client onWritable!";
    if (!sent_) {
      sent_ = true;
      LOG(INFO) << "sending.";
      auto buffPtr = makeStupidBuffer(msg_);
      sendBuff(buffPtr, [](const ErrBack::except_option &ex) {
        LOG(INFO) << "client sent buffer.";
        if (ex.hasValue()) {
          LOG(INFO) << "client err: " << ex.value().what();
        }
      });
    }
  }
  void onEOF() override {
    LOG(INFO) << "onEOF.";
  }
};

static const short kPortNum = 9051;

void runServer() {
  auto react = ReactorThread::createShared();
  EpollReactor::Options options;
  options.setWaitTimeout(std::chrono::milliseconds(200));
  react->start(options);
  ThreadBaton bat1;
  react->runInEventThread([&bat1, react]() {
    LOG(INFO) << "here.";
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
  ThreadBaton bat2;
  react->runInEventThread([&bat2, react]() {
    SocketAddr addr {"127.0.0.1", kPortNum};
    auto client = TCPClient::connectPtr(
      react->getReactor(), new ClientHandler, addr
    );
    react->addTask(client->getEpollTask(), [&bat2]() {
      LOG(INFO) << "added client.";
      bat2.post();
    });
  });
  bat2.wait();
  this_thread::sleep_for(chrono::milliseconds(20000));
  react->runInEventThread([react]() {
    react->stop([](const ErrBack::except_option& err) {
      CHECK(!err.hasValue());
      LOG(INFO) << "stopped";
    });
  });
  react->join();
  LOG(INFO) << "runServer() joined.";
}


int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start.";
  runServer();
  LOG(INFO) << "end.";
}
