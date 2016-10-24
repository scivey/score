#pragma once

#include <unordered_map>
#include "score/Unit.h"
#include "score/Try.h"
#include "score/func/Function.h"
#include "score/util/misc.h"
#include "score/async/wheel/HTimerWheel.h"
#include "score/async/SignalHandlerRegistry.h"
#include "score/async/queues/MPSCEventChannel.h"
#include "score/async/queues/EventDataChannel.h"
#include "score/async/CallbackEvent.h"



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
  using EventDataChannel = queues::EventDataChannel;

  struct ControlMessage {
    work_cb_t work;
    ControlMessage(work_cb_t&&);
    ControlMessage();
  };
  using control_channel_t = queues::MPSCEventChannel<ControlMessage>;
  using control_channel_ptr_t = std::unique_ptr<control_channel_t>;

  using data_channel_t = EventDataChannel;
  using data_channel_ptr_t = std::shared_ptr<data_channel_t>;

  struct EventDataChannelHandle {
    std::unique_ptr<CallbackEvent> readEvent {nullptr};
    data_channel_ptr_t channel {nullptr};
  };

 protected:
  base_ptr_t base_ {nullptr};
  wheel_ptr_t wheel_ {nullptr};
  sig_registry_ptr_t sigRegistry_ {nullptr};
  control_channel_ptr_t controlChannel_ {nullptr};
  std::unique_ptr<CallbackEvent> controlEvent_ {nullptr};
  using data_channel_map_t = std::unordered_map<std::thread::id, EventDataChannelHandle>;
  data_channel_map_t dataChannels_;

  EventContext();
  void drainControlChannel();
  void bindControlChannel();
  void drainDataChannel(const data_channel_ptr_t&);
  void bindDataChannel(data_channel_ptr_t);
 public:
  base_t* getBase();
  wheel_t* getWheel();
  sig_registry_t* getSignalRegistry();
  static EventContext* createNew();
  void runSoon(work_cb_t&& func);
  Try<Unit> threadsafeTrySendControlMessage(ControlMessage&& msg);
  Try<Unit> threadsafeRegisterDataChannel(std::shared_ptr<EventDataChannel>);
};

}} // score::async
