#pragma once
#include <thread>
#include <chrono>
#include "score/func/Function.h"
#include "score/Try.h"
#include "score/util/misc.h"
#include "score/MoveWrapper.h"
#include "score/util/try_helpers.h"
#include "score/exceptions/ScoreError.h"
#include "score/async/EventError.h"
#include "score/async/queues/QueueError.h"
#include "score/async/EventContext.h"
#include "score/async/queues/MPMCQueue.h"
#include "score/async/queues/EventDataChannel.h"
#include "score/async/tpool/ThreadPoolError.h"
#include "score/async/tpool/Task.h"

namespace score { namespace async { namespace tpool {


class ThreadPoolWorker {
 public:
  using queue_t = queues::MPMCQueue<std::unique_ptr<Task>>;

 protected:
  using EventDataChannel = queues::EventDataChannel;
  using sender_id_t = std::thread::id;
  using sender_channel_t = std::shared_ptr<EventDataChannel>;
  using data_channel_map_t = std::unordered_map<sender_id_t, sender_channel_t>;
  queue_t *workQueue_ {nullptr};
  std::unique_ptr<std::thread> thread_ {nullptr};
  std::atomic<bool> running_ {false};
  data_channel_map_t dataChannels_;

  sender_channel_t& getDataChannel(sender_id_t senderId, EventContext *ctx) {
    auto found = dataChannels_.find(senderId);
    if (found != dataChannels_.end()) {
      return found->second;
    }
    auto newChannel = EventDataChannel::createSharedAsSender();
    ctx->threadsafeRegisterDataChannel(newChannel).throwIfFailed();
    while (!newChannel->hasReceiverAcked()) {
      ;
    }
    dataChannels_.insert(std::make_pair(senderId, newChannel));
    found = dataChannels_.find(senderId);
    DCHECK(found != dataChannels_.end());
    return found->second;
  }


  Try<Unit> runTask(std::unique_ptr<Task> task) {
    Try<Unit> taskOutcome;
    try {
      task->run();
      taskOutcome = util::makeTrySuccess<Unit>();
    } catch (const std::exception& ex) {
      taskOutcome = util::makeTryFailure<Unit, TaskThrewException>(ex.what());
    }
    auto senderContext = task->getSenderContext();
    auto senderId = task->getSenderId();
    auto wrappedTask = makeMoveWrapper(std::move(task));
    auto wrappedOutcome = makeMoveWrapper(std::move(taskOutcome));
    EventDataChannel::Message message {
      [wrappedTask, wrappedOutcome]() {
        MoveWrapper<Try<Unit>> movedOutcome = wrappedOutcome;
        Try<Unit> unwrappedOutcome = movedOutcome.move();
        MoveWrapper<std::unique_ptr<Task>> movedTask = wrappedTask;
        std::unique_ptr<Task> unwrappedTask = movedTask.move();
        if (unwrappedOutcome.hasException()) {
          unwrappedTask->onError(std::move(unwrappedOutcome.exception()));
        } else {
          unwrappedTask->onSuccess();
        }
      }
    };
    auto dataChannel = getDataChannel(senderId, senderContext);
    auto sendResult = dataChannel->getQueue()->trySend(std::move(message));
    if (sendResult.hasException()) {
      return util::makeTryFailure<Unit, CouldntSendResult>(
        sendResult.exception().what()
      );
    }
    return util::makeTrySuccess<Unit>();
  }
  void threadFunc() {
    std::unique_ptr<Task> task;
    const int64_t kWaitMicroseconds {100000}; // 100ms
    while (running_.load()) {
      if (workQueue_->wait_dequeue_timed(task, kWaitMicroseconds)) {
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
