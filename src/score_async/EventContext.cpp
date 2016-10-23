#include "score_async/EventContext.h"
#include "score_async/EvBase.h"
#include "score_async/wheel/HTimerWheel.h"
#include "score_async/SignalHandlerRegistry.h"
#include "score/func/Function.h"
#include "score/ScopeGuard.h"
#include "score/util/misc.h"

namespace score { namespace async {

using base_t = typename EventContext::base_t;
using wheel_t = typename EventContext::wheel_t;
using sig_registry_t = typename EventContext::sig_registry_t;

EventContext::EventContext(){}

base_t* EventContext::getBase() {
  return base_.get();
}

wheel_t* EventContext::getWheel() {
  return wheel_.get();
}

sig_registry_t* EventContext::getSignalRegistry() {
  return sigRegistry_.get();
}

EventContext* EventContext::createNew() {
  auto ctx = new EventContext;
  auto guard = score::makeGuard([ctx]() {
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

void EventContext::runSoon(work_cb_t&& work) {
  work_cb_t toRun { std::forward<work_cb_t>(work)};
  wheel_->addOneShot(std::move(toRun), std::chrono::milliseconds{5});
}

}} // score::async
