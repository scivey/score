#pragma once
#include <thread>
#include <chrono>
#include "score/func/Function.h"
#include "score/Try.h"
#include "score/util/misc.h"
#include "score/MoveWrapper.h"
#include "score/util/try_helpers.h"
#include "score/exceptions/ScoreError.h"
#include "score_async/EventError.h"
#include "score_async/queues/QueueError.h"
#include "score_async/EventContext.h"
#include "score_async/queues/MPMCQueue.h"
#include "score_async/queues/EventDataChannel.h"
#include "score_async/tpool/ThreadPoolError.h"
#include "score_async/tpool/Task.h"
#include "score_async/tpool/ThreadPoolWorker.h"

namespace score { namespace async { namespace tpool {


class ThreadPool {
 public:
  using queue_t = queues::MPMCQueue<std::unique_ptr<Task>>;
 protected:
  size_t numThreads_ {0};
  std::unique_ptr<queue_t> workQueue_ {nullptr};
  std::vector<std::shared_ptr<ThreadPoolWorker>> workers_;
  std::atomic<bool> running_ {false};
  ThreadPool(size_t numThreads, std::unique_ptr<queue_t> workQueue)
    : numThreads_(numThreads), workQueue_(std::move(workQueue)){}

 public:
  static ThreadPool* createNew(size_t nThreads) {
    return new ThreadPool{
      nThreads,
      util::makeUnique<queue_t>(size_t{50000})
    };
  }
  bool isRunning() const {
    return running_.load();
  }

  Try<Unit> triggerStop() {
    for (;;) {
      if (!running_.load()) {
        return util::makeTryFailure<Unit, NotRunning>("NotRunning");
      }
      bool expected = true;
      bool desired = false;
      if (running_.compare_exchange_strong(expected, desired)) {
        stopWorkers();
        return util::makeTrySuccess<Unit>();
      }
    }
  }

 protected:
  void stopWorkers() {
    for (auto& worker: workers_) {
      worker->triggerStop();
    }
  }
  void startWorkers() {
    DCHECK(numThreads_ >= 1);
    for (size_t i = 0; i < numThreads_; i++) {
      auto worker = std::make_shared<ThreadPoolWorker>(
        workQueue_.get()
      );
      auto startOutcome = worker->start();
      DCHECK(!startOutcome.hasException()) << startOutcome.exception().what();
      workers_.push_back(std::move(worker));
    }
  }
 public:
  Try<Unit> start() {
    if (numThreads_ < 1) {
      return util::makeTryFailure<Unit, InvalidSettings>(
        "ThreadPool needs at least 1 thread."
      );
    }
    for (;;) {
      if (running_.load()) {
        return util::makeTryFailure<Unit, AlreadyRunning>("AlreadyRunning");
      }
      bool expected = false;
      bool desired = true;
      if (running_.compare_exchange_strong(expected, desired)) {
        startWorkers();
        return util::makeTrySuccess<Unit>();
      }
    }
  }
  void join() {
    for (auto& worker: workers_) {
      worker->join();
    }
  }
  Try<Unit> stopJoin() {
    auto stopResult = triggerStop();
    if (stopResult.hasException()) {
      return stopResult;
    }
    join();
    return util::makeTrySuccess<Unit>();
  }
  Try<Unit> trySubmit(std::unique_ptr<Task>&& task) {
    if (!isRunning()) {
      return util::makeTryFailure<Unit, NotRunning>("Not running.");
    }
    if (workQueue_->try_enqueue(std::move(task))) {
      return util::makeTrySuccess<Unit>();
    }
    return util::makeTryFailure<Unit, queues::QueueFull>("Queue Full.");
  }
  ~ThreadPool() {
    triggerStop(); // ignore potential NotRunning error
    join();
  }
};

}}} // score::async::tpool
