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

#include "score/ScopeGuard.h"
#include "score/locks/Synchronized.h"
#include "score/locks/ThreadBaton.h"
#include "score/async/ErrBack.h"
#include "score/async/VoidCallback.h"
#include "score/Maybe.h"
#include "score/MoveWrapper.h"
#include "score/FixedBuffer.h"
#include "score/exceptions/exceptions.h"
#include "score/exceptions/macros.h"
#include "score/reactor/FileDescriptor.h"
#include "score/reactor/EpollFd.h"
#include "score/net/SocketAddr.h"
#include "score/reactor/EpollReactor.h"
#include "score/net/TCPSocket.h"
#include "score/reactor/ClientSocketTask.h"
#include "score/reactor/AcceptSocketTask.h"
#include "score/reactor/ServerSocketTask.h"
#include "score/reactor/ReactorThread.h"

using namespace std;
using namespace score::locks;
using namespace score::reactor;
using score::async::ErrBack;
using score::async::VoidCallback;
using score::MoveWrapper;
using score::Buffer;
using score::exceptions::BaseError;
using score::exceptions::SystemError;


TEST(TestEpollReactor, TestConstruction) {
  auto reactor = EpollReactor::create();
  EXPECT_FALSE(reactor.isRunning());
}

TEST(TestEpollReactor, TestRun1) {
  auto reactor = EpollReactor::create();
  reactor.runOnce();
  EXPECT_TRUE(true);
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
}


TEST(TestEpollReactor, TestRun3) {
  auto reactorThread = ReactorThread::createShared();
  reactorThread->start();
  auto sock = TCPSocket::bindPort(9025);
  auto task = std::unique_ptr<AcceptSocketTask>(
    new AcceptSocketTask(std::move(sock))
  );
  task->getSocket().listen();
  ThreadBaton bat1;
  reactorThread->addTask(task.get(), [&bat1](){
    bat1.post();
  });
  bat1.wait();
  ThreadBaton bat2;
  reactorThread->runInEventThread([&task, &reactorThread, &bat2]() {
    task->getSocket().stop();
    reactorThread->stop([&bat2](const score::Maybe<std::exception> &err) {
      bat2.post();
    });
  });
  bat2.wait();
  reactorThread->join();
}


