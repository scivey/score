#include "score_async/EvBaseCore.h"
#include "score/util/misc.h"
#include "score/TimerSettings.h"
#include <memory>
#include <event2/event.h>


namespace score { namespace async {

using base_t = EvBaseCore::base_t;

void EvBaseCore::takeOwnershipOfBase(base_t *base) {
  base_ = score::util::asDestructorPtr<base_t>(base, event_base_free);
}

EvBaseCore EvBaseCore::createWithOwnership(base_t *base) {
  EvBaseCore baseCore;
  baseCore.takeOwnershipOfBase(base);
  return baseCore;
}

EvBaseCore EvBaseCore::create() {
  return createWithOwnership(event_base_new());
}

EvBaseCore* EvBaseCore::newWithOwnership(base_t *base) {
  return new EvBaseCore {EvBaseCore::createWithOwnership(base)};
}

EvBaseCore* EvBaseCore::createNew() {
  return newWithOwnership(event_base_new());
}

base_t* EvBaseCore::getBase() {
  return base_.get();
}

void EvBaseCore::runOnce() {
  event_base_loop(base_.get(), EVLOOP_ONCE);
}

void EvBaseCore::runNonBlocking() {
  event_base_loop(base_.get(), EVLOOP_NONBLOCK);
}

void EvBaseCore::runForever() {
  event_base_dispatch(base_.get());
}

void EvBaseCore::runFor(timeval *tv) {
  event_base_loopexit(base_.get(), tv);
  event_base_dispatch(base_.get());
}

void EvBaseCore::runFor(const TimerSettings &settings) {
  timeval tv = settings.toTimeVal();
  runFor(&tv);
}

}} // score::async
