#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include <chrono>
#include "score_async/EventContext.h"
#include "score_async/ThreadPool.h"

using namespace score;
using namespace score::async;
using score::func::Function;
using namespace std;

TEST(TestThreadPool, TestSanity1) {
  auto evCtx = util::createShared<EventContext>();
  std::atomic<bool> workRan {false};
  std::atomic<bool> doneCallbackRan {false};
  auto pool = util::createShared<ThreadPool>(size_t{4});
  auto task = Task::createFromEventThread(evCtx.get(),
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

  LOG(INFO) << "here.";
  bool didWorkRun {false};
  bool didCallbackRun {false};
  for (;;) {
    auto workResult = workRan.load();
    if (workResult != didWorkRun) {
      LOG(INFO) << "workRan";
      didWorkRun = workResult;
    }
    auto doneResult = doneCallbackRan.load();
    if (doneResult != didCallbackRun) {
      LOG(INFO) << "done cb ran";
      didCallbackRun = doneResult;
    }
    if (didWorkRun && didCallbackRun) {
      break;
    }
    evCtx->getBase()->runOnce();
  }
  LOG(INFO) << "here.";
  EXPECT_TRUE(workRan.load());
  EXPECT_TRUE(doneCallbackRan.load());
}

