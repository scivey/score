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
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <folly/io/async/EventBase.h>
#include "score/Try.h"
#include "score/Unit.h"
#include "score/util/misc.h"
#include "score/func/Function.h"
#include "score_async/tpool/ThreadPool.h"
#include "score_async/EventContext.h"

using namespace std;
using score::Try;
using score::Unit;
using score::func::Function;
namespace util = score::util;
using namespace score::async::tpool;
using namespace score::async;

using bench_func_t = score::func::Function<void>;

double runBenched(bench_func_t&& benchFunc, size_t nIter) {
  std::vector<double> durations;
  durations.reserve(nIter * 2);
  for (size_t i = 0; i < nIter; i++) {
    auto start = chrono::high_resolution_clock::now();
    benchFunc();
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(
      end - start
    );
    durations.push_back(elapsed.count());
  }
  return std::accumulate(durations.begin(), durations.end(), 0.0) / ((double) nIter);
}

static const size_t kNumTasks = 100;
static const size_t kNumThreads = 4;
static const size_t kIterations = 10;


template<typename T>
class BaseTask {
 protected:
  mutable std::atomic<size_t> *taskCounter_ {nullptr};
  mutable size_t *doneCounter_ {nullptr};
  mutable bool *isDone_ {nullptr};
  T* getThis() const {
    return (T*) this;
  }
 public:
  BaseTask(std::atomic<size_t> *taskCount, size_t *doneCount,
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

class ScoreTask: public BaseTask<ScoreTask> {
 public:
  ScoreTask(std::atomic<size_t>* x, size_t* y, bool* z): BaseTask<ScoreTask>(x, y, z){}
};


void benchScoreThreadpool() {
  auto ctx = util::createShared<EventContext>();
  auto pool = util::createShared<ThreadPool>(kNumThreads);
  pool->start().throwIfFailed();
  auto benchFunc = [ctx, pool]() {
    std::atomic<size_t> workCounter {0};
    size_t doneCounter {0};
    bool finishedCycle {false};
    // auto incrWork = [&workCounter]() {
    //   workCounter++;
    // };
    // auto incrDone = [&doneCounter, &finishedCycle](Try<Unit> result) {
    //   CHECK(!result.hasException()) << "exception: " << result.exception().what();
    //   doneCounter++;
    //   if (doneCounter == kNumTasks) {
    //     finishedCycle = true;
    //   }
    // };
    for (size_t i = 0; i < kNumTasks; i++) {
      ScoreTask scoreTask {&workCounter, &doneCounter, &finishedCycle};
      auto submitResult = pool->trySubmit(Task::createFromEventThread(
        ctx.get(),
        [scoreTask]() {
          scoreTask.doWork();
        },
        [scoreTask](Try<Unit> result) {
          result.throwIfFailed();
          scoreTask.doFinished();
        }
      ));
      CHECK(!submitResult.hasException()) << "exception: " << submitResult.exception().what();
    }
    while (!finishedCycle) {
      ctx->getBase()->runNonBlocking();
    }
  };
  auto duration = runBenched(benchFunc, kIterations);
  LOG(INFO) << "score duration: " << duration;
}


class WangleTask: public BaseTask<WangleTask> {
 protected:
  mutable folly::EventBase* evBase_ {nullptr};
 public:
  WangleTask(std::atomic<size_t>* x, size_t* y, bool* z): BaseTask<WangleTask>(x, y, z){}

  void setBase(folly::EventBase *base) {
    evBase_ = base;
  }
};


void benchWangleThreadpool() {
  folly::EventBase evBase;
  wangle::CPUThreadPoolExecutor pool {kNumThreads};
  auto benchFunc = [&evBase, &pool]() {
    std::atomic<size_t> workCounter {0};
    size_t doneCounter {0};
    bool isDone {false};
    for (size_t i = 0; i < kNumTasks; i++) {
      WangleTask task {&workCounter, &doneCounter, &isDone};
      task.setBase(&evBase);
      pool.add([task, &evBase]() {
        task.doWork();
        evBase.runInEventBaseThread([task]() {
          task.doFinished();
        });
      });
    }
    while (!isDone) {
      evBase.loopOnce(EVLOOP_NONBLOCK);
    }
  };
  auto duration = runBenched(benchFunc, kIterations);
  LOG(INFO) << "wangle duration: " << duration;
}


int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start";
  benchScoreThreadpool();
  benchWangleThreadpool();

  LOG(INFO) << "end";
}
