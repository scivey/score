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

namespace score { namespace async { namespace tpool {

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


}}} // score::async::tpool
