#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include <chrono>
#include "score/async/EventContext.h"
#include "score/async/tpool/ThreadPool.h"
#include "score/async/tpool/CallbackTask.h"

using namespace score;
using namespace score::async;
using namespace score::async::tpool;
using score::func::Function;
using namespace std;

TEST(TestThreadPool, TestSanity1) {
  auto evCtx = util::createShared<EventContext>();
  std::atomic<bool> workRan {false};
  std::atomic<bool> doneCallbackRan {false};
  auto pool = util::createShared<ThreadPool>(size_t{4});
  auto task = CallbackTask::createFromEventThread(evCtx.get(),
    [&workRan]() mutable {
      workRan.store(true);
    },
    [&doneCallbackRan](Try<Unit> outcome) mutable {
      outcome.throwIfFailed();
      doneCallbackRan.store(true);
    }
  );
  pool->start().throwIfFailed();
  pool->trySubmit(std::move(task)).throwIfFailed();
  bool didWorkRun {false};
  bool didCallbackRun {false};
  for (;;) {
    auto workResult = workRan.load();
    if (workResult != didWorkRun) {
      didWorkRun = workResult;
    }
    auto doneResult = doneCallbackRan.load();
    if (doneResult != didCallbackRun) {
      didCallbackRun = doneResult;
    }
    if (didWorkRun && didCallbackRun) {
      break;
    }
    evCtx->getBase()->runOnce();
  }
  EXPECT_TRUE(workRan.load());
  EXPECT_TRUE(doneCallbackRan.load());
}

