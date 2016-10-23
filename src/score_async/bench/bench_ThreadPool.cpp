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
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
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


class BenchResult {
 protected:
  std::string name_ {"Default"};
  double mean_ {0.0};
  double stdev_ {0.0};
  double min_ {0.0};
  double max_ {0.0};
 public:
  BenchResult(){}
  BenchResult(const std::string& name)
    : name_(name) {}
  BenchResult& setMean(double mean) {
    mean_ = mean;
    return *this;
  }
  BenchResult& setStdev(double stdev) {
    stdev_ = stdev;
    return *this;
  }
  BenchResult& setMin(double minVal) {
    min_ = minVal;
    return *this;
  }
  BenchResult& setMax(double maxVal) {
    max_ = maxVal;
    return *this;
  }
  BenchResult& setName(const std::string& name) {
    name_ = name;
    return *this;
  }
  double getMin() const {
    return min_;
  }
  double getMax() const {
    return max_;
  }
  double getMean() const {
    return mean_;
  }
  double getStdev() const {
    return stdev_;
  }
  const std::string& getName() const {
    return name_;
  }
};

std::ostream& operator<<(std::ostream& oss, const BenchResult& benched) {
  oss << "\n\t[" << benched.getName() << "]" << endl;
  oss << "\t\tmean:  " << benched.getMean() << endl
      << "\t\tstdev: " << benched.getStdev() << endl
      << "\t\tmin:   " << benched.getMin() << endl
      << "\t\tmax:   " << benched.getMax() << endl
      << endl;
  return oss;
}

BenchResult runBenched(const string& name, bench_func_t&& benchFunc, size_t nIter) {
  boost::accumulators::accumulator_set<
    double,
    boost::accumulators::features<
      boost::accumulators::tag::mean,
      boost::accumulators::tag::variance,
      boost::accumulators::tag::max,
      boost::accumulators::tag::min
    >
  > accum;
  for (size_t i = 0; i < nIter; i++) {
    auto start = chrono::high_resolution_clock::now();
    benchFunc();
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(
      end - start
    );
    accum(elapsed.count());
  }
  return BenchResult(name)
    .setMean(boost::accumulators::mean(accum))
    .setStdev(sqrt(boost::accumulators::variance(accum)))
    .setMin(boost::accumulators::min(accum))
    .setMax(boost::accumulators::max(accum));
}

static const size_t kNumTasks = 1000;
static const size_t kNumThreads = 8;
static const size_t kIterations = 100;


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




void benchScoreThreadpool() {
  auto ctx = util::createShared<EventContext>();
  auto pool = util::createShared<ThreadPool>(kNumThreads);
  pool->start().throwIfFailed();
  auto benchFunc = [ctx, pool]() {
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
  auto result = runBenched("score", benchFunc, kIterations);
  LOG(INFO) << result;
}


class WangleTask: public BasePoolBenchTask<WangleTask> {
 protected:
  mutable folly::EventBase* evBase_ {nullptr};
 public:
  WangleTask(std::atomic<size_t>* x, size_t* y, bool* z): BasePoolBenchTask<WangleTask>(x, y, z){}

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
  auto result = runBenched("wangle", benchFunc, kIterations);
  LOG(INFO) << result;
}


int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start";
  benchScoreThreadpool();
  benchWangleThreadpool();

  LOG(INFO) << "end";
}
