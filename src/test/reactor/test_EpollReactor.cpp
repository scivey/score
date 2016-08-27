#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>
#include <fcntl.h>
#include <netdb.h>
#include <cstdio>
#include <cstdlib>

#include <stdexcept>
#include <vector>
#include <queue>
#include <atomic>

#include <glog/logging.h>
#include <gtest/gtest.h>

#include "aliens/ScopeGuard.h"
#include "aliens/locks/Synchronized.h"
#include "aliens/async/ErrBack.h"
#include "aliens/async/VoidCallback.h"
#include "aliens/Maybe.h"
#include "aliens/MoveWrapper.h"
#include "aliens/FixedBuffer.h"
#include "aliens/exceptions/exceptions.h"
#include "aliens/exceptions/macros.h"
#include "aliens/reactor/FileDescriptor.h"
#include "aliens/reactor/EpollFd.h"
#include "aliens/reactor/SocketAddr.h"
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/TCPSocket.h"
#include "aliens/reactor/ClientSocketTask.h"
#include "aliens/reactor/AcceptSocketTask.h"
#include "aliens/reactor/ServerSocketTask.h"

using namespace std;
using aliens::async::ErrBack;
using aliens::async::VoidCallback;
using aliens::locks::Synchronized;
using aliens::MoveWrapper;
using aliens::Buffer;
using aliens::exceptions::BaseError;
using aliens::exceptions::SystemError;

using aliens::reactor::TCPSocket;
using aliens::reactor::FileDescriptor;
using aliens::reactor::EpollFd;
using aliens::reactor::EpollReactor;
using aliens::reactor::SocketAddr;
using aliens::reactor::ServerSocketTask;
using aliens::reactor::ClientSocketTask;
using aliens::reactor::AcceptSocketTask;













class ReactorThread : public std::enable_shared_from_this<ReactorThread> {
 protected:
  std::unique_ptr<EpollReactor> reactor_ {nullptr};
  std::unique_ptr<std::thread> thread_ {nullptr};
  ReactorThread(){}
  std::atomic<bool> running_ {false};
  aliens::Maybe<ErrBack> onStopped_;
  Synchronized<std::queue<VoidCallback>> toRun_;
 public:
  static std::shared_ptr<ReactorThread> createShared() {
    std::shared_ptr<ReactorThread> instance{new ReactorThread};
    instance->reactor_ = EpollReactor::createUnique();
    return instance;
  }
  EpollReactor* getReactor() {
    return reactor_.get();
  }
  bool isRunning() const {
    return running_.load();
  }
  void start() {
    CHECK(!isRunning());
    bool expected = false;
    bool desired = true;
    if (running_.compare_exchange_strong(expected, desired)) {
      auto self = shared_from_this();
      thread_.reset(new std::thread([this, self]() {
        while (isRunning()) {
          reactor_->runForDuration(std::chrono::milliseconds(50));
          auto queueHandle = toRun_.getHandle();
          while (!queueHandle->empty()) {
            VoidCallback func = std::move(queueHandle->front());
            queueHandle->pop();
            func();
          }
        }
        if (onStopped_.hasValue()) {
          onStopped_.value()();
        }
      }));
    } else {
      throw BaseError("already started!");
    }
  }
  void stop(ErrBack &&cb) {
    if (!isRunning()) {
      cb(BaseError("not running!"));
      return;
    }
    onStopped_.assign(std::move(cb));
    bool expected = true;
    bool desired = false;
    if (running_.compare_exchange_strong(expected, desired)) {
      ; // do nothing; cb will be called by the loop in `start()`
    } else {
      onStopped_.value()(BaseError("someone else stopped before we could. race condition?"));
    }
  }

 protected:
  // NB must be called from event loop thread.
  void addTaskImpl(EpollReactor::Task *task) {
    reactor_->addTask(task);
  }

 public:
  void addTask(EpollReactor::Task *task) {
    auto self = shared_from_this();
    runInEventThread([self, this, task]() {
      addTaskImpl(task);
    });
  }
  void addTask(EpollReactor::Task *task, VoidCallback &&cb) {
    auto self = shared_from_this();
    auto cbWrapper = aliens::makeMoveWrapper(std::forward<VoidCallback>(cb));
    runInEventThread([self, this, task, cbWrapper]() {
      addTaskImpl(task);
      aliens::MoveWrapper<VoidCallback> unwrapped = cbWrapper;
      VoidCallback movedCb = unwrapped.move();
      movedCb();
    });
  }
  void runInEventThread(VoidCallback &&cb) {
    auto handle = toRun_.getHandle();
    handle->push(std::move(cb));
  }
  void runInEventThread(VoidCallback &&cb, ErrBack &&onFinish) {
    auto cbWrapper = aliens::makeMoveWrapper(
      std::forward<VoidCallback>(cb)
    );
    auto doneWrapper = aliens::makeMoveWrapper(
      std::forward<ErrBack>(onFinish)
    );
    auto self = shared_from_this();
    runInEventThread([this, self, cbWrapper, doneWrapper]() {
      MoveWrapper<VoidCallback> movedCb = cbWrapper;
      VoidCallback unwrappedCb = movedCb.move();
      MoveWrapper<ErrBack> movedDoneCb = doneWrapper;
      ErrBack unwrappedDoneCb = movedDoneCb.move();
      bool raised = false;
      try {
        unwrappedCb();
      } catch (const std::exception &ex) {
        raised = true;
        unwrappedDoneCb(ex);
      }
      if (!raised) {
        unwrappedDoneCb();
      }
    });
  }
  void join() {
    if (!isRunning()) {
      return;
    }
    while (running_.load()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    thread_->join();
  }
};

TEST(TestEpollReactor, TestConstruction) {
  auto reactor = EpollReactor::create();
  EXPECT_FALSE(reactor.isRunning());
}

TEST(TestEpollReactor, TestRun1) {
  auto reactor = EpollReactor::create();
  reactor.runOnce();
  EXPECT_TRUE(true);
  LOG(INFO) << "here";
}

TEST(TestTCPSocket, TestSanity) {
  auto sock = TCPSocket::bindPort(9019);
  CHECK(sock.valid());
}



TEST(TestEpollReactor, TestRun2) {
  auto reactor = EpollReactor::create();
  auto sock = TCPSocket::bindPort(9022);
  auto task = std::unique_ptr<AcceptSocketTask>(
    new AcceptSocketTask(std::move(sock))
  );
  task->getSocket().listen();
  reactor.addTask(task.get());
  reactor.runForDuration(std::chrono::milliseconds{20});
  EXPECT_TRUE(true);
  task->getSocket().stop();
  LOG(INFO) << "here";
}


TEST(TestEpollReactor, TestRun3) {
  auto reactorThread = ReactorThread::createShared();
  reactorThread->start();
  auto sock = TCPSocket::bindPort(9025);
  auto task = std::unique_ptr<AcceptSocketTask>(
    new AcceptSocketTask(std::move(sock))
  );
  task->getSocket().listen();
  reactorThread->addTask(task.get(), [](){
    LOG(INFO) << "added task!";
  });
  LOG(INFO) << "sleeping...";
  this_thread::sleep_for(chrono::milliseconds(100));
  std::atomic<bool> done {false};
  reactorThread->runInEventThread([&task, &reactorThread, &done]() {
    task->getSocket().stop();
    reactorThread->stop([&done](const aliens::Maybe<std::exception> &err) {
      done.store(true);
    });
  });
  LOG(INFO) << "waiting for stop..";
  reactorThread->join();
  LOG(INFO) << "end.";
}


void joinAtomic(std::atomic<bool> &done) {
  while (!done.load()) {
    this_thread::sleep_for(chrono::milliseconds(50));
  }
}

class ClientHandler : public ClientSocketTask::EventHandler {
 public:
  void onConnectSuccess() override {
    LOG(INFO) << "onConnectSuccess!";
  }
  void onConnectError(const std::exception &ex) override {
    LOG(INFO) << "onConnectError : " << ex.what();
  }
  void onWritable() override {
    LOG(INFO) << "onWritable.";
  }
  void onReadable() override {
    LOG(INFO) << "onWritable.";
  }
};

TEST(TestEpollReactor, TestRun4) {
  auto reactorThread = ReactorThread::createShared();
  reactorThread->start();
  auto sock = TCPSocket::bindPort(9025);
  auto task = std::unique_ptr<AcceptSocketTask>(
    new AcceptSocketTask(std::move(sock))
  );
  task->getSocket().listen();
  std::atomic<bool> serverStarted {false};
  reactorThread->addTask(task.get(), [&serverStarted](){
    LOG(INFO) << "added task!";
    serverStarted.store(true);
  });
  joinAtomic(serverStarted);
  LOG(INFO) << "sleeping...";
  this_thread::sleep_for(chrono::milliseconds(200));


  auto clientSock = TCPSocket::connect(SocketAddr("127.0.0.1", 9025));
  auto clientTask = new ClientSocketTask(std::move(clientSock), new ClientHandler);
  reactorThread->addTask(clientTask, []() {
    LOG(INFO) << "added client!";
  });
  LOG(INFO) << "sleeping.....";
  this_thread::sleep_for(chrono::milliseconds(1000));
  std::atomic<bool> done {false};
  LOG(INFO) << "stopping..";
  reactorThread->runInEventThread([&task, &reactorThread, &done]() {
    LOG(INFO) << "here..";
    task->getSocket().stop();
    reactorThread->stop([&done](const aliens::Maybe<std::exception> &err) {
      done.store(true);
    });
  });
  LOG(INFO) << "waiting for stop..";
  reactorThread->join();
  LOG(INFO) << "end.";
}