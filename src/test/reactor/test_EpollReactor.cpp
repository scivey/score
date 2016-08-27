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
#include "aliens/reactor/ReactorThread.h"

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
using aliens::reactor::ReactorThread;













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


