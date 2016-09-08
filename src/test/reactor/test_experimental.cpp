#include <gtest/gtest.h>
#include <atomic>
#include <memory>
#include <chrono>
#include "score/posix/FileDescriptor.h"
#include "score/reactor/ReactorThread.h"
#include "score/reactor/TimerFd.h"
#include "score/exceptions/macros.h"
#include "score/FixedBuffer.h"
#include "score/mem/util.h"
#include "score/locks/ThreadBaton.h"


using namespace score::locks;
using namespace score::mem;
using namespace score::async;
using namespace score::posix;
using namespace score::reactor;
using namespace std;
using score::Buffer;


class TimerHandler: public TimerFd::EventHandler {
 public:
  using counter_t = std::atomic<size_t>;
 protected:
  counter_t &counter_;
  ThreadBaton &bat_;
 public:
  TimerHandler(counter_t &counter, ThreadBaton &bat)
    : counter_(counter), bat_(bat) {}
  void onTick() {
    LOG(INFO) << "tick.";
    if (counter_.fetch_add(1) == 5) {
      bat_.post();
      stop();
    }
  }
};

TEST(TestReactorExperimental, TestSomething) {
  auto reactor = ReactorThread::createShared();
  reactor->start();
  ThreadBaton bat1, bat2;
  std::atomic<size_t> counter {0};
  std::atomic<TimerFd*> timerPtr {nullptr};
  TimerSettings settings {
    chrono::milliseconds {1000},
    chrono::milliseconds {1000}
  };
  reactor->runInEventThread([&bat1, &bat2, &timerPtr, reactor, &counter, &settings]() {
    auto timerFd = TimerFd::Factory::createShared(
      settings, new TimerHandler(counter, bat2)
    );
      LOG(INFO) << "here!";
    auto fd2 = timerFd;
    // timerPtr.store(timerFd.get());
    reactor->pushOnFinished([fd2]() {
      LOG(INFO) << "pushOnFinished!";
    });
    reactor->addTask(timerFd->getEpollTask(), [&bat1, fd2]() {
      LOG(INFO) << "here.";
    });
  });
  bat2.wait();
  ThreadBaton bat3;
  reactor->runInEventThread([reactor, &bat3]() {
    // timerFd->stop();
    reactor->stop([&bat3](const ErrBack::except_option &err) {
      EXPECT_FALSE(err.hasValue());
      bat3.post();
    });
  });
  bat3.wait();
  reactor->join();
  EXPECT_TRUE(true);
}
