#pragma once

#include <memory>
#include <event2/event.h>
#include "score/util/misc.h"
#include "score/TimerSettings.h"

namespace score { namespace async {

class EvBase {
 public:
  using base_t = event_base;
  using base_ptr_t = typename score::util::unique_destructor_ptr<base_t>::type;

 protected:
  base_ptr_t base_ {nullptr};
  void takeOwnershipOfBase(base_t *base);
 public:
  static EvBase createWithOwnership(base_t *base);
  static EvBase create();
  static EvBase* newWithOwnership(base_t *base);
  static EvBase* createNew();

  base_t* getBase();
  void runOnce();
  void runNonBlocking();
  void runForever();
  void runFor(timeval *tv);
  void runFor(const TimerSettings &settings);
};

}} // score::async
