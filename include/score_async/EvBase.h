#pragma once

#include <memory>
#include <event2/event.h>
#include <queue>
#include "score/util/misc.h"
#include "score/TimerSettings.h"
#include "score_async/EvBaseCore.h"

namespace score { namespace async {

class EvBase {
 public:
  using base_t = event_base;
  using base_core_t = EvBaseCore;
 protected:
  base_core_t base_;
  EvBase(){}
 public:
  EvBase(base_core_t&& core): base_(std::move(core)) {}
  static EvBase create() {
    return EvBase{EvBaseCore::create()};
  }
  static EvBase* createNew() {
    return new EvBase(EvBase::create());
  }
  base_t* getBase() {
    return base_.getBase();
  }
  void runForever() {
    base_.runForever();
  }
  void runFor(timeval *tv) {
    base_.runFor(tv);
  }
  void runFor(const TimerSettings &settings) {
    timeval tv = settings.toTimeVal();
    runFor(&tv);
  }
  void runOnce() {
    base_.runNonBlocking();
  }
};

}} // score::async
