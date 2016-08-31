#include <gtest/gtest.h>
#include <atomic>
#include <memory>
#include <sys/timerfd.h>

#include "score/reactor/ReactorThread.h"
#include "score/posix/FileDescriptor.h"
#include "score/reactor/TimerFd.h"
#include "score/exceptions/macros.h"
#include "score/FixedBuffer.h"
#include "score/mem/util.h"

using namespace score::mem;
using namespace score::async;
using namespace score::posix;
using namespace std;
using score::Buffer;


using namespace score::reactor;

class Handler : public TimerFd::EventHandler {
 protected:
  std::atomic<size_t> &target_;
 public:
  Handler(std::atomic<size_t> &target): target_(target){}
  void onTick() override {
    target_.fetch_add(1);
  }
};



namespace {
void joinAtomic(std::atomic<bool> &done) {
  while (!done.load()) {
    ;
  }
}
}

TEST(TestTimerFd, TestSanity) {
  auto reactorThread = ReactorThread::createShared();
  reactorThread->start();
  std::atomic<size_t> target {0};
  auto handler = makeUnique<Handler>(target);
  auto timer = TimerFd::Factory::createShared(
    TimerSettings(chrono::milliseconds(5), chrono::milliseconds(5)),
    handler.get()
  );
  reactorThread->addTask(timer->getEpollTask(), [](){});

  size_t current = target.load();
  while (current < 5) {
    ;
    current = target.load();
  }
  std::atomic<bool> finished {false};
  reactorThread->runInEventThread([timer, reactorThread, &finished]() {
    timer->stop();
    reactorThread->stop([timer, reactorThread, &finished](const ErrBack::except_option &err) {
      EXPECT_FALSE(err.hasValue());
      finished.store(true);
    });
  });
  joinAtomic(finished);
  reactorThread->join();
  EXPECT_GT(target.load(), 4);
}