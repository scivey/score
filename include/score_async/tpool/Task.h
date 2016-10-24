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
  virtual ~Task() = default;
};

}}} // score::async::tpool
