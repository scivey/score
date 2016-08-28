#include <gtest/gtest.h>
#include <atomic>
#include <memory>
#include <sys/timerfd.h>

#include "aliens/reactor/ReactorThread.h"
#include "aliens/reactor/FileDescriptor.h"
#include "aliens/reactor/ClientSocketTask.h"
#include "aliens/reactor/AcceptSocketTask.h"
#include "aliens/reactor/TimerFd.h"
#include "aliens/exceptions/macros.h"
#include "aliens/FixedBuffer.h"
#include "aliens/mem/util.h"

using namespace aliens::mem;
using namespace aliens::async;

using namespace std;
using aliens::Buffer;


using namespace aliens::reactor;

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
  auto timer = TimerFd::createShared(
    TimerSettings(chrono::milliseconds(5), chrono::milliseconds(5)),
    handler.get()
  );
  reactorThread->addTask(timer->getEpollTask(), [](){
    LOG(INFO) << "added task!";
  });

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
  LOG(INFO) << "waiting for stop..";
  reactorThread->join();
  EXPECT_GT(target.load(), 4);
  LOG(INFO) << "end.";
}