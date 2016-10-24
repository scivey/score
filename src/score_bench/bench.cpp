#include "score_bench/bench.h"
#include <chrono>
#include <cmath>
#include "score_bench/BenchResult.h"
#include "score/func/Function.h"
#include "score/Optional.h"
#include "score/Unit.h"
#include "score/Try.h"
#include "score/util/try_helpers.h"

using namespace std;

namespace score { namespace bench {

template<typename TCollection>
double calculateVariance(double average, const TCollection& collection) {
  double acc {0.0};
  for (auto item: collection) {
    double asDouble = (double) item;
    acc += pow(average - asDouble, 2);
  }
  return acc / ((double) collection.size());
}


template<typename T>
class SimpleAccumulator {
 public:
  class NoData: public std::runtime_error {
   public:
    NoData(): std::runtime_error("No data!") {}
  };
  struct Stats {
    T maxVal {0};
    T minVal {0};
    T sum {0};
    size_t count {0};
    double mean {0};
    double variance {0};
    double stdev {0};
  };
 protected:
  Optional<Stats> stats_;
  std::vector<T> points_;
  Try<Unit> calculateStats() {
    if (points_.empty()) {
      return util::makeTryFailure<Unit, NoData>();
    }
    Stats stats;
    stats.count = points_.size();
    bool haveMin {false};
    bool haveMax {false};
    for (auto elem: points_) {
      stats.sum += elem;
      if (!haveMin || elem < stats.minVal) {
        stats.minVal = elem;
        haveMin = true;
      }
      if (!haveMax || elem > stats.maxVal) {
        stats.maxVal = elem;
        haveMax = true;
      }
    }
    stats.mean = ((double) stats.sum) / ((double) stats.count);
    stats.variance = calculateVariance(stats.mean, points_);
    stats.stdev = sqrt(stats.variance);
    stats_.assign(std::move(stats));
    return util::makeTrySuccess<Unit>();
  }
 public:
  void reserve(size_t hint) {
    points_.reserve(hint);
  }
  void push(T elem) {
    points_.push_back(elem);
  }
  Try<Stats> getStats() {
    if (!stats_) {
      auto outcome = calculateStats();
      if (outcome.hasException()) {
        return Try<Stats>{ std::move(outcome.exception()) };
      }
    }
    CHECK(!!stats_);
    return util::makeTrySuccess<Stats>(stats_.value());
  }
};

BenchResult runBenched(const std::string& name, bench_func_t&& benchFunc, size_t nIter) {
  SimpleAccumulator<double> accum;
  for (size_t i = 0; i < nIter; i++) {
    auto start = chrono::high_resolution_clock::now();
    benchFunc();
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(
      end - start
    );
    accum.push(elapsed.count());
  }
  auto stats = accum.getStats().value();
  return BenchResult(name)
    .setMean(stats.mean)
    .setStdev(stats.stdev)
    .setMin(stats.minVal)
    .setMax(stats.maxVal);
}

}} // score::bench
