#include <algorithm>
#include <vector>
#include <list>
#include <array>
#include <memory>
#include <random>
#include <functional>
#include <numeric>
#include <thread>
#include <chrono>
#include <glog/logging.h>
#include "score/Try.h"
#include "score/Unit.h"
#include "score/util/misc.h"
#include "score/func/Function.h"
#include "score/async/tpool/ThreadPool.h"
#include "score/async/EventContext.h"

using namespace std;
using score::Try;
using score::Unit;
using score::func::Function;
namespace util = score::util;
using namespace score::async::tpool;
using namespace score::async;

static const size_t kNumThreads = 4;
static const size_t kNumTasks = 1000;
static const size_t kIterations = 50;


template<typename T>
class BasePoolBenchTask {
 protected:
  mutable std::atomic<size_t> *taskCounter_ {nullptr};
  mutable size_t *doneCounter_ {nullptr};
  mutable bool *isDone_ {nullptr};
  T* getThis() const {
    return (T*) this;
  }
 public:
  BasePoolBenchTask(std::atomic<size_t> *taskCount, size_t *doneCount,
      bool *doneFlag)
    : taskCounter_(taskCount),
      doneCounter_(doneCount),
      isDone_(doneFlag) {}

  void doWork() const {
    taskCounter_->fetch_add(1);
  }

  void doFinished() const {
    (*doneCounter_)++;
    size_t nDone = *doneCounter_;
    if (nDone == kNumTasks) {
      *isDone_ = true;
    }
  }
};

class ScoreTask: public BasePoolBenchTask<ScoreTask> {
 public:
  ScoreTask(std::atomic<size_t>* x, size_t* y, bool* z): BasePoolBenchTask<ScoreTask>(x, y, z){}
};


void runThreadpool() {
  auto ctx = util::createShared<EventContext>();
  auto pool = util::createShared<ThreadPool>(kNumThreads);
  pool->start().throwIfFailed();

  auto runMe = [ctx, pool]() {
    std::atomic<size_t> workCounter {0};
    size_t doneCounter {0};
    bool finishedCycle {false};
    for (size_t i = 0; i < kNumTasks; i++) {
      ScoreTask benchTask {&workCounter, &doneCounter, &finishedCycle};
      auto task = CallbackTask::createFromEventThread(
        ctx.get(),
        [benchTask]() {
          benchTask.doWork();
        },
        [benchTask](Try<Unit> result) {
          result.throwIfFailed();
          benchTask.doFinished();
        }
      );
      auto submitResult = pool->trySubmit(std::move(task));
      CHECK(!submitResult.hasException()) << "exception: " << submitResult.exception().what();
    }
    while (!finishedCycle) {
      ctx->getBase()->runNonBlocking();
    }
  };
  for (size_t i = 0; i < kIterations; i++) {
    LOG(INFO) << i;
    runMe();
  }
}

int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start";
  runThreadpool();
  LOG(INFO) << "end";
}
