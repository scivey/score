#pragma once

#include <memory>
#include <event2/event.h>
#include "score/TimerSettings.h"
#include "score_async/EvBaseCore.h"

namespace score { namespace async {

class EvBase {
 public:
  using base_t = event_base;
  using base_core_t = EvBaseCore;
 protected:
  base_core_t base_;
  EvBase();
 public:
  EvBase(base_core_t&& core);
  static EvBase create();
  static EvBase* createNew();
  base_t* getBase();
  void runForever();
  void runFor(timeval *tv);
  void runFor(const TimerSettings &settings);
  void runOnce();
};

}} // score::async
