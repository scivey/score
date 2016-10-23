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

class TaskBase {
 public:
  using ctx_t = EventContext;
  using sender_id_t = std::thread::id;
 protected:
  ctx_t *senderContext_ {nullptr};
  sender_id_t senderId_ {0};
 public:
  bool hasGoodContext() const {
    return !!senderContext_ && senderId_ > sender_id_t{0};
  }
  bool valid() const {
    return hasGoodContext() && good();
  }
  void setSenderContext(ctx_t *ctx) {
    senderContext_ = ctx;
  }
  void setSenderId(sender_id_t id) {
    senderId_ = id;
  }
  sender_id_t getSenderId() {
    return senderId_;
  }
  ctx_t* getSenderContext() {
    return senderContext_;
  }
  virtual bool good() const = 0;
  virtual void run() = 0;
  virtual void onSuccess() noexcept = 0;
  virtual void onError(score::ExceptionWrapper&& ex) noexcept = 0;
  virtual ~TaskBase() = default;
};

class CallbackTask: public TaskBase {
 public:
  using work_cb_t = func::Function<void>;
  using done_cb_t = func::Function<void, Try<Unit>>;
 protected:
  work_cb_t work_;
  done_cb_t onFinished_;
 public:
  CallbackTask(work_cb_t&& workCb, done_cb_t&& doneCb)
    : work_(std::forward<work_cb_t>(workCb)), onFinished_(std::forward<done_cb_t>(doneCb)){}
  bool good() const override {
    return !!work_ && !!onFinished_;
  }
  void run() override {
    work_();
  }
  void onSuccess() noexcept override {
    onFinished_(util::makeTrySuccess<Unit>());
  }
  void onError(score::ExceptionWrapper&& ex) noexcept override {
    onFinished_(score::Try<Unit> {std::forward<score::ExceptionWrapper>(ex)});
  }
  static std::unique_ptr<CallbackTask> createFromEventThread(EventContext *ctx,
      work_cb_t&& work, done_cb_t&& onFinished) {
    auto task = util::makeUnique<CallbackTask>(
      std::forward<work_cb_t>(work), std::forward<done_cb_t>(onFinished)
    );
    task->setSenderContext(ctx);
    task->setSenderId(std::this_thread::get_id());
    return task;
  }
};

}}} // score::async::tpool
