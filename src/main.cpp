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
#include "aliens/reactor/TCPServerSocket.h"
#include "aliens/locks/ThreadBaton.h"

#include "aliens/async/ErrBack.h"
#include "aliens/async/VoidCallback.h"
#include "aliens/async/Callback.h"
#include "aliens/exceptions/macros.h"
#include "aliens/macros.h"
#include <signal.h>
#include <folly/Demangle.h>
#include <folly/Conv.h>

#include <glog/logging.h>

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



class SomeRequestHandler : public TCPServerSocket::EventHandler {
 public:
  void getReadBuffer(void **buff, size_t *buffLen, size_t hint) override {
    void *buffPtr = malloc(hint);
    memset(buffPtr, 0, hint);
    *buff = buffPtr;
    *buffLen = hint;
  }
  void readBufferAvailable(void *buff, size_t buffLen) override {
    LOG(INFO) << "readBufferAvailable : " << buffLen;
    auto data = (const char*) buff;
    LOG(INFO) << "\tread: '" << data << "'";
  }
  void onWritable() override {
    LOG(INFO) << "onWritable.";
  }
  void onEOF() override {
    LOG(INFO) << "onEOF.";
  }
};



class RequestHandlerFactory {
 public:
  virtual TCPServerSocket::EventHandler* getHandler() = 0;
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
    SocketAddr localAddr("127.0.0.1", 9999);
    SocketAddr remoteAddr(hostName, portNo);
    auto handler = handlerFactory_->getHandler();
    auto serverSock = new TCPServerSocket(TCPServerSocket::fromAccepted(
      std::move(desc), handler, localAddr, remoteAddr
    ));
    ALIENS_UNUSED(serverSock);
  }
  void onAcceptError(int err) override {
    LOG(INFO) << "onAcceptError [" << err << "]";
  }
};

class SomeFactory: public RequestHandlerFactory {
 public:
  TCPServerSocket::EventHandler* getHandler() override {
    return new SomeRequestHandler;
  }
};

void runServer() {
  auto react = ReactorThread::createShared();
  react->start();
  this_thread::sleep_for(chrono::milliseconds(2000));
  auto acceptor = new TCPAcceptSocket(TCPAcceptSocket::bindPort(
    9051, new AcceptHandler(std::unique_ptr<SomeFactory>(new SomeFactory))
  ));
  acceptor->listen();
  this_thread::sleep_for(chrono::milliseconds(2000));
  react->addTask(acceptor->getEpollTask(), []() {
    LOG(INFO) << "added acceptor.";
  });
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

#include <type_traits>

template<typename T>
using decay_t = typename std::decay<T>::type;

template<typename T>
using res_t = typename std::result_of<T ()>::type;

int get9() {
  return 9;
}

int add5(int x) {
  return x + 5;
}

using Noise = aliens::test_support::Noisy<31>;


static const size_t kBuffBytes = 32;
using Buff = aliens::FixedBuffer<kBuffBytes>;

#include <cstdlib>
#include <cstdio>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>

void tryIov() {
  const size_t kNBuffs = 4;
  Buff buffs[kNBuffs];
  iovec vecs[kNBuffs];
  for (size_t i = 0; i < kNBuffs; i++) {
    vecs[i].iov_base = buffs[i].vdata();
    vecs[i].iov_len = buffs[i].capacity() - 1;
  }
  int fd = open("CMakeLists.txt", O_RDONLY | O_CLOEXEC);
  ssize_t nr = readv(fd, (iovec*) vecs, kNBuffs);
  LOG(INFO) << "nr : " << nr;
  ALIENS_CHECK_SYSCALL2(nr, "readv()");
  for (size_t i = 0; i < kNBuffs; i++) {
    LOG(INFO) << buffs[i].copyToString();
  }
}


#include "aliens/atomic/IntrinsicAtomic.h"
#include "aliens/formatters/formatters.h"
using namespace aliens::formatters;
using AtomicUint128 = aliens::atomic::IntrinsicAtomic<__uint128_t>;


void tryAtomic16() {
  AtomicUint128 anAtom;
  std::vector<std::shared_ptr<thread>> threads;
  for (size_t i = 0; i < 4; i++) {
    threads.push_back(std::make_shared<std::thread>([&anAtom]() {
      for (size_t j = 0; j < 50; j++) {
        for (;;) {
          auto expected = anAtom.load();
          auto desired = expected + 1;
          if (anAtom.compare_exchange_strong(&expected, desired)) {
            break;
          }
        }
      }
    }));
  }
  for (auto &t: threads) {
    t->join();
  }
  auto lastVal = anAtom.load();
  LOG(INFO) << "final : " << lastVal;
}

int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start.";
  tryAtomic16();
  LOG(INFO) << "end.";
}
