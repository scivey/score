#include "score_async/EvBase.h"


namespace score { namespace async {

using base_t = typename EvBase::base_t;
using base_core_t = typename EvBase::base_core_t;

EvBase::EvBase(){}

EvBase::EvBase(base_core_t&& core): base_(std::move(core)) {}

EvBase EvBase::create() {
  return EvBase{EvBaseCore::create()};
}

EvBase* EvBase::createNew() {
  return new EvBase(EvBase::create());
}

base_t* EvBase::getBase() {
  return base_.getBase();
}
void EvBase::runForever() {
  base_.runForever();
}
void EvBase::runFor(timeval *tv) {
  base_.runFor(tv);
}
void EvBase::runFor(const TimerSettings &settings) {
  timeval tv = settings.toTimeVal();
  runFor(&tv);
}
void EvBase::runOnce() {
  base_.runNonBlocking();
}

}} // score::async
