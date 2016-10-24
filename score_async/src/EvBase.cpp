#include "score/async/EvBase.h"
#include "score/util/misc.h"
#include "score/TimerSettings.h"
#include <memory>
#include <event2/event.h>


namespace score { namespace async {

using base_t = EvBase::base_t;

void EvBase::takeOwnershipOfBase(base_t *base) {
  base_ = score::util::asDestructorPtr<base_t>(base, event_base_free);
}

EvBase EvBase::createWithOwnership(base_t *base) {
  EvBase baseCore;
  baseCore.takeOwnershipOfBase(base);
  return baseCore;
}

EvBase EvBase::create() {
  return createWithOwnership(event_base_new());
}

EvBase* EvBase::newWithOwnership(base_t *base) {
  return new EvBase {EvBase::createWithOwnership(base)};
}

EvBase* EvBase::createNew() {
  return newWithOwnership(event_base_new());
}

base_t* EvBase::getBase() {
  return base_.get();
}

void EvBase::runOnce() {
  event_base_loop(base_.get(), EVLOOP_ONCE);
}

void EvBase::runNonBlocking() {
  event_base_loop(base_.get(), EVLOOP_ONCE | EVLOOP_NONBLOCK);
}

void EvBase::runForever() {
  event_base_dispatch(base_.get());
}

void EvBase::runFor(timeval *tv) {
  event_base_loopexit(base_.get(), tv);
  event_base_dispatch(base_.get());
}

void EvBase::runFor(const TimerSettings &settings) {
  timeval tv = settings.toTimeVal();
  runFor(&tv);
}

}} // score::async
