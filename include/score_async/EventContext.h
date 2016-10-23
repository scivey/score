#pragma once

#include "score/Unit.h"
#include "score/Try.h"
#include "score/func/Function.h"
#include "score/util/misc.h"
#include "score_async/wheel/HTimerWheel.h"
#include "score_async/SignalHandlerRegistry.h"
#include "score_async/queues/MPSCEventChannel.h"
#include "score_async/CallbackEvent.h"



namespace score { namespace async {

class EvBase;

class EventContext {
 public:
  using base_t = EvBase;
  using base_ptr_t = std::unique_ptr<base_t>;
  using wheel_t = wheel::HTimerWheel;
  using wheel_ptr_t = std::unique_ptr<wheel_t>;
  using sig_registry_t = SignalHandlerRegistry;
  using sig_registry_ptr_t = std::unique_ptr<SignalHandlerRegistry>;
  using work_cb_t = func::Function<void>;

  struct ControlMessage {
    work_cb_t work;
    ControlMessage(work_cb_t&&);
    ControlMessage();
  };
  using control_channel_t = queues::MPSCEventChannel<ControlMessage>;
  using control_channel_ptr_t = std::unique_ptr<control_channel_t>;

 protected:
  base_ptr_t base_ {nullptr};
  wheel_ptr_t wheel_ {nullptr};
  sig_registry_ptr_t sigRegistry_ {nullptr};
  control_channel_ptr_t controlChannel_ {nullptr};
  std::unique_ptr<CallbackEvent> controlEvent_ {nullptr};
  EventContext();
  void bindControlChannel();
 public:
  base_t* getBase();
  wheel_t* getWheel();
  sig_registry_t* getSignalRegistry();
  static EventContext* createNew();
  void runSoon(work_cb_t&& func);
  Try<Unit> threadsafeTrySendControlMessage(ControlMessage&& msg);
};

}} // score::async
