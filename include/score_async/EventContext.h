#pragma once

#include "score_async/EvBase.h"
#include "score_async/wheel/HTimerWheel.h"
#include "score_async/SignalHandlerRegistry.h"
#include "score_async/VoidCallback.h"
#include "score/util/misc.h"
#include <folly/ScopeGuard.h>

namespace score { namespace async {

class EventContext {
 public:
  using base_t = EvBase;
  using base_ptr_t = std::unique_ptr<base_t>;
  using wheel_t = wheel::HTimerWheel;
  using wheel_ptr_t = std::unique_ptr<wheel_t>;
  using sig_registry_t = SignalHandlerRegistry;
  using sig_registry_ptr_t = std::unique_ptr<SignalHandlerRegistry>;
  using work_cb_t = VoidCallback;
 protected:
  base_ptr_t base_ {nullptr};
  wheel_ptr_t wheel_ {nullptr};
  sig_registry_ptr_t sigRegistry_ {nullptr};
  EventContext(){}
 public:
  base_t* getBase() {
    return base_.get();
  }
  wheel_t* getWheel() {
    return wheel_.get();
  }
  sig_registry_t* getSignalRegistry() {
    return sigRegistry_.get();
  }
  static EventContext* createNew() {
    auto ctx = new EventContext;
    auto guard = folly::makeGuard([ctx]() {
      delete ctx;
    });
    ctx->base_ = util::createUnique<base_t>();
    wheel_t::WheelSettings wheelSettings {
      {10, 10},
      {100, 10},
      {1000, 60},
      {60000, 60}
    };
    ctx->wheel_ = util::createUnique<wheel_t>(ctx->base_.get(), wheelSettings);
    ctx->wheel_->start();
    ctx->sigRegistry_ = util::createUnique<sig_registry_t>(ctx->base_.get());
    guard.dismiss();
    return ctx;
  }
  template<typename TCallable>
  void runSoon(TCallable&& callable) {
    work_cb_t cb { std::forward<TCallable>(callable) };
    wheel_->addOneShot(std::move(cb), std::chrono::milliseconds{5});
  }
  template<typename TCallable>
  void runSoon(const TCallable& callable) {
    work_cb_t cb { callable };
    wheel_->addOneShot(std::move(cb), std::chrono::milliseconds{5});
  }
};

}} // score::async
