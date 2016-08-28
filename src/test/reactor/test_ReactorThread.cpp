#include <gtest/gtest.h>
#include <atomic>
#include <memory>

#include "aliens/reactor/ReactorThread.h"
#include "aliens/reactor/ClientSocketTask.h"
#include "aliens/reactor/AcceptSocketTask.h"
#include "aliens/FixedBuffer.h"
#include "aliens/mem/util.h"

using namespace aliens::mem;
using namespace aliens::reactor;
using namespace aliens::async;

using namespace std;
using aliens::Buffer;

namespace {

void joinAtomic(std::atomic<bool> &done) {
  while (!done.load()) {
    ;
  }
}

class ClientHandler1 : public ClientSocketTask::EventHandler {
 public:
  void onConnectSuccess() override {}
  void onConnectError(const std::exception &ex) override {}
  void onWritable() override {}
  void onReadable() override {}
};
}

TEST(TestReactorThread, TestRun1) {
  auto reactorThread = ReactorThread::createShared();
  reactorThread->start();
  auto sock = TCPSocket::bindPort(9025);
  auto task = std::shared_ptr<AcceptSocketTask>(
    new AcceptSocketTask(std::move(sock))
  );
  task->getSocket().listen();
  std::atomic<bool> serverStarted {false};
  reactorThread->addTask(task.get(), [&serverStarted](){
    serverStarted.store(true);
  });
  joinAtomic(serverStarted);
  auto clientSock = TCPSocket::connect(SocketAddr("127.0.0.1", 9025));
  auto clientTask = new ClientSocketTask(std::move(clientSock), new ClientHandler1);
  std::atomic<bool> addedClient {false};
  reactorThread->addTask(clientTask, [&addedClient]() {
    addedClient.store(true);
  });
  joinAtomic(addedClient);
  std::atomic<bool> done {false};
  reactorThread->runInEventThread([task, reactorThread, &done]() {
    task->getSocket().stop();
    reactorThread->stop([&done, reactorThread, task](const aliens::Maybe<std::exception> &err) {
      done.store(true);
    });
  });
  joinAtomic(done);
  reactorThread->join();
}

class ClientHandler2 : public ClientSocketTask::EventHandler {
 protected:
  std::unique_ptr<Buffer> toSend_;
  std::atomic<bool> connected_ {false};
  std::atomic<bool> wrote_ {false};
 public:
  ClientHandler2(std::unique_ptr<Buffer> &&toSend)
    : toSend_(std::forward<std::unique_ptr<Buffer>>(toSend)){}
  void onConnectSuccess() override {
    connected_.store(true);
  }
  void onConnectError(const std::exception &ex) override {
    throw ex;
  }
  void onWritable() override {
    if (toSend_) {
      write(std::move(toSend_), [this](const ErrBack::except_option &err) {
        EXPECT_FALSE(err.hasValue());
        wrote_.store(true);
      });
    }
  }
  void onReadable() override {
    LOG(INFO) << "onReadable";
  }
};


TEST(TestReactorThread, TestRun2) {
  auto reactorThread = ReactorThread::createShared();
  reactorThread->start();
  auto sock = TCPSocket::bindPort(9025);
  auto task = std::unique_ptr<AcceptSocketTask>(
    new AcceptSocketTask(std::move(sock))
  );
  task->getSocket().listen();
  std::atomic<bool> serverStarted {false};
  reactorThread->addTask(task.get(), [&serverStarted](){
    serverStarted.store(true);
  });
  joinAtomic(serverStarted);
  auto clientSock = TCPSocket::connect(SocketAddr("127.0.0.1", 9025));
  auto buff = makeUnique<Buffer>();
  buff->fillWith("this is a test\r\n");
  auto clientTask = new ClientSocketTask(
    std::move(clientSock),
    new ClientHandler2(std::move(buff))
  );
  std::atomic<bool> addedTask {false};
  reactorThread->addTask(clientTask, [&addedTask]() {
    addedTask.store(true);
  });
  joinAtomic(addedTask);
  std::atomic<bool> done {false};
  reactorThread->runInEventThread([&task, &reactorThread, &done]() {
    task->getSocket().stop();
    reactorThread->stop([&done](const aliens::Maybe<std::exception> &err) {
      done.store(true);
    });
  });
  reactorThread->join();
}