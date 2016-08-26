#include <glog/logging.h>
#include <thread>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "aliens/async/ThreadExecutorCore.h"
#include "aliens/locks/ThreadBaton.h"

using namespace aliens::async;
using aliens::locks::ThreadBaton;
using namespace std;

TEST(TestThreadExecutorCore, TestWorks1) {
  ThreadBaton bat1, bat2;
  auto core = std::make_shared<ThreadExecutorCore>();
  EXPECT_FALSE(core->isRunning());
  LOG(INFO) << "pre-start";
  thread worker([&bat1, &bat2, &core]() {
    core->start([&bat1, &core](const ErrBack::except_option &err) {
      LOG(INFO) << "here1";
      EXPECT_FALSE(err.hasValue());
      EXPECT_TRUE(core->isRunning());
      LOG(INFO) << "about to bat1.post()";
      bat1.post();
      LOG(INFO) << "bat1.post() done.";

    });
    LOG(INFO) << "about to bat1.wait()";
    bat1.wait();
    LOG(INFO) << "bat1.wait() done";
    core->stop([&bat2](const ErrBack::except_option &err) {
      EXPECT_FALSE(err.hasValue());
      LOG(INFO) << "about to bat2.post()";
      bat2.post();
      LOG(INFO) << "bat2.post() done.";
    });
  });
  LOG(INFO) << "about to bat2.wait()";
  bat2.wait();
  LOG(INFO) << "bat2.wait() done.";

  LOG(INFO) << "about to join worker.";
  worker.join();
  LOG(INFO) << "joined worker.";
}


#define WAIT_FOR(x) { \
  LOG(INFO) << "about to x.wait()"; x.wait(); LOG(INFO) << "x.wait() done."; \
}

TEST(TestThreadExecutorCore, TestWorks2) {
  ThreadBaton bat1, bat2, bat3, bat4;
  auto core = std::make_shared<ThreadExecutorCore>();
  EXPECT_FALSE(core->isRunning());
  std::atomic<size_t> someCounter {0};
  std::atomic<bool> reachedEnd {false};
  thread bgThread([&bat1, &bat2, &bat3, &bat4, &core, &someCounter, &reachedEnd]() {
    core->start([&bat1](const ErrBack::except_option &ex) {
      EXPECT_FALSE(ex.hasValue());
      bat1.post();
    });
    bat1.wait();
    EXPECT_TRUE(core->isRunning());
    core->submit(
      [&someCounter]() {
        someCounter.store(9017);
      },
      [&bat2, &reachedEnd, &core](const ErrBack::except_option &err) {
        EXPECT_FALSE(err.hasValue());
        bat2.post();
      }
    );
    bat2.wait();
    core->stop([&bat3, &reachedEnd](const ErrBack::except_option &err) {
      reachedEnd.store(true);
      bat3.post();
    });
    WAIT_FOR(bat3);
    bat4.post();
  });
  WAIT_FOR(bat4);

  LOG(INFO) << "about to bgThread.join()";
  bgThread.join();
  LOG(INFO) << "did bgThread.join()";
  EXPECT_FALSE(core->isRunning());
  EXPECT_EQ(9017, someCounter.load());
  EXPECT_TRUE(reachedEnd.load());
  LOG(INFO) << "here..";
}

