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

namespace score { namespace async {

SCORE_DECLARE_EXCEPTION(ThreadPoolError, EventError);

class Task {
 public:
  SCORE_DECLARE_EXCEPTION(TaskError, ThreadPoolError);
  SCORE_DECLARE_EXCEPTION(InvalidTask, TaskError);
  SCORE_DECLARE_EXCEPTION(TaskThrewException, TaskError);
  SCORE_DECLARE_EXCEPTION(CouldntSendResult, TaskError);

  using work_cb_t = func::Function<void>;
  using done_cb_t = func::Function<void, Try<Unit>>;
  using ctx_t = EventContext;
  using sender_id_t = std::thread::id;
 protected:
  ctx_t *senderCtx_ {nullptr};
  sender_id_t senderId_ {0};
  work_cb_t work_;
  done_cb_t onFinished_;

 public:
  Task(){}
  Task(EventContext* senderCtx, sender_id_t senderId,
        work_cb_t&& work, done_cb_t&& onFinished)
    : senderCtx_(senderCtx),
      senderId_(senderId),
      work_(std::forward<work_cb_t>(work)),
      onFinished_(std::forward<done_cb_t>(onFinished)) {}

  bool good() const {
    return !!work_ && !!onFinished_ && !!senderCtx_ && (senderId_ > std::thread::id{0});
  }
  Try<Unit> run() {
    if (!good()) {
      return util::makeTryFailure<Unit, InvalidTask>(
        "Task is in invalid state."
      );
    }
    Try<Unit> outcome;
    try {
      work_();
      outcome = util::makeTrySuccess<Unit>();
    } catch (const std::exception& ex) {
      outcome = util::makeTryFailure<Unit, TaskThrewException>(ex.what());
    }
    auto onFinishedWrapper = score::makeMoveWrapper(std::move(onFinished_));
    auto outcomeWrapper = score::makeMoveWrapper(std::move(outcome));
    auto sendResult = senderCtx_->threadsafeTrySendControlMessage(
      EventContext::ControlMessage {
        [onFinishedWrapper, outcomeWrapper]() {
          MoveWrapper<Try<Unit>> movedOutcome = outcomeWrapper;
          Try<Unit> unwrappedOutcome = movedOutcome.move();
          MoveWrapper<done_cb_t> movedOnFinished = onFinishedWrapper;
          done_cb_t unwrappedOnFinished = movedOnFinished.move();
          unwrappedOnFinished(std::move(unwrappedOutcome));
        }
      }
    );
    if (sendResult.hasException()) {
      return util::makeTryFailure<Unit, CouldntSendResult>(
        sendResult.exception().what()
      );
    }
    return util::makeTrySuccess<Unit>();
  }
  static Task createFromEventThread(EventContext *ctx,
      work_cb_t&& work, done_cb_t&& onFinished) {
    return Task {
      ctx,
      std::this_thread::get_id(),
      std::forward<work_cb_t>(work),
      std::forward<done_cb_t>(onFinished)
    };
  }
};


class ThreadPool {
 public:
  using queue_t = queues::MPMCQueue<Task>;
  SCORE_DECLARE_EXCEPTION(NotRunning, ThreadPoolError);
  SCORE_DECLARE_EXCEPTION(InvalidSettings, ThreadPoolError);
  SCORE_DECLARE_EXCEPTION(AlreadyRunning, ThreadPoolError);
 protected:
  size_t numThreads_ {0};
  std::unique_ptr<queue_t> workQueue_ {nullptr};
  std::vector<std::shared_ptr<std::thread>> threads_;
  std::atomic<bool> running_ {false};
  ThreadPool(size_t numThreads, std::unique_ptr<queue_t> workQueue)
    : numThreads_(numThreads), workQueue_(std::move(workQueue)){}

  void workerThreadFunc() {
    Task task;
    while (running_.load()) {
      if (workQueue_->try_dequeue(task)) {
        task.run().throwIfFailed();
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
 public:
  static ThreadPool* createNew(size_t nThreads) {
    return new ThreadPool{
      nThreads,
      util::makeUnique<queue_t>(size_t{20000})
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
        return util::makeTrySuccess<Unit>();
      }
    }
  }

 protected:
  void startThreads() {
    DCHECK(numThreads_ >= 1);
    for (size_t i = 0; i < numThreads_; i++) {
      threads_.push_back(std::make_shared<std::thread>([this](){
        this->workerThreadFunc();
      }));
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
        startThreads();
        return util::makeTrySuccess<Unit>();
      }
    }
  }
  void join() {
    for (auto& workerThread: threads_) {
      workerThread->join();
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
  Try<Unit> trySubmit(Task&& task) {
    if (!isRunning()) {
      return util::makeTryFailure<Unit, NotRunning>("Not running.");
    }
    if (workQueue_->try_enqueue(std::forward<Task>(task))) {
      return util::makeTrySuccess<Unit>();
    }
    return util::makeTryFailure<Unit, queues::QueueFull>("Queue Full.");
  }
  ~ThreadPool() {
    triggerStop(); // ignore potential NotRunning error
    join();
  }
};

}}