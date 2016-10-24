#pragma once
#include <thread>
#include <chrono>
#include "score/func/Function.h"
#include "score/Try.h"
#include "score/util/misc.h"
#include "score/MoveWrapper.h"
#include "score/util/try_helpers.h"
#include "score_async/queues/QueueError.h"
#include "score_async/EventContext.h"
#include "score_async/tpool/ThreadPoolError.h"

namespace score { namespace async { namespace tpool {

class CallbackTask: public Task {
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
