#include <gtest/gtest.h>
#include <atomic>
#include "aliens/reactor/ReactorThread.h"
#include "aliens/reactor/ClientSocketTask.h"
#include "aliens/reactor/AcceptSocketTask.h"

using namespace std;
using namespace aliens::reactor;

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

TEST(TestReactorThread, TestRun) {
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