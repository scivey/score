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

namespace score { namespace async { namespace tpool {


class ThreadPoolWorker {
 public:
  using queue_t = queues::MPMCQueue<std::unique_ptr<TaskBase>>;

 protected:
  queue_t *workQueue_ {nullptr};
  std::unique_ptr<std::thread> thread_ {nullptr};
  std::atomic<bool> running_ {false};

  Try<Unit> runTask(std::unique_ptr<TaskBase> task) {
    Try<Unit> taskOutcome;
    try {
      task->run();
      taskOutcome = util::makeTrySuccess<Unit>();
    } catch (const std::exception& ex) {
      taskOutcome = util::makeTryFailure<Unit, TaskThrewException>(ex.what());
    }
    auto senderContext = task->getSenderContext();
    auto wrappedTask = makeMoveWrapper(std::move(task));
    auto wrappedOutcome = makeMoveWrapper(std::move(taskOutcome));
    EventContext::ControlMessage message {
      [wrappedTask, wrappedOutcome]() {
        MoveWrapper<Try<Unit>> movedOutcome = wrappedOutcome;
        Try<Unit> unwrappedOutcome = movedOutcome.move();
        MoveWrapper<std::unique_ptr<TaskBase>> movedTask = wrappedTask;
        std::unique_ptr<TaskBase> unwrappedTask = movedTask.move();
        if (unwrappedOutcome.hasException()) {
          unwrappedTask->onError(std::move(unwrappedOutcome.exception()));
        } else {
          unwrappedTask->onSuccess();
        }
      }
    };
    auto sendResult = senderContext->threadsafeTrySendControlMessage(
      std::move(message)
    );
    if (sendResult.hasException()) {
      return util::makeTryFailure<Unit, CouldntSendResult>(
        sendResult.exception().what()
      );
    }
    return util::makeTrySuccess<Unit>();
  }
  void threadFunc() {
    std::unique_ptr<TaskBase> task;
    while (running_.load()) {
      if (workQueue_->try_dequeue(task)) {
        if (task->valid()) {
          runTask(std::move(task)).throwIfFailed();
        } else {
          LOG(INFO) << "received invalid task.";
        }
      }
    }
  }

  void startThread() {
    DCHECK(running_.load());
    DCHECK(!thread_);
    thread_.reset(new std::thread([this]() {
      this->threadFunc();
    }));
  }

 public:
  ThreadPoolWorker(queue_t *workQueue): workQueue_(workQueue){}
  static ThreadPoolWorker* createNew(queue_t *workQueue) {
    return new ThreadPoolWorker(workQueue);
  }
  Try<Unit> start() {
    for (;;) {
      if (running_.load()) {
        return util::makeTryFailure<Unit, AlreadyRunning>("AlreadyRunning");
      }
      bool expected = false;
      bool desired = true;
      if (running_.compare_exchange_strong(expected, desired)) {
        startThread();
        return util::makeTrySuccess<Unit>();
      }
    }
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
  void join() {
    if (thread_) {
      thread_->join();
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
};

}}} // score::async::tpool
