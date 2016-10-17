#pragma once

#include <memory>
#include <vector>
#include <glog/logging.h>
#include "evs/events2/EvBase.h"
#include "evs/events2/TimerEvent.h"
#include "evs/events2/VoidCallback.h"
#include "evs/events/TimerSettings.h"
#include "evs/events2/wheel/WheelBuffer.h"
#include "evs/events2/wheel/SimpleTimerWheel.h"
#include "evs/macros.h"

namespace evs { namespace events2 { namespace wheel {

class HTimerWheel1 {
 public:
  using subsecond_wheel_t = SimpleTimerWheel<100, 10>;
  using second_wheel_t = SimpleTimerWheel<1000, 60>;
  using base_t = typename second_wheel_t::base_t;
  using timer_settings_t = typename second_wheel_t::timer_settings_t;
 protected:
  base_t *base_ {nullptr};
  std::unique_ptr<second_wheel_t> secondWheel_ {nullptr};
  std::unique_ptr<subsecond_wheel_t> subsecondWheel_ {nullptr};

  HTimerWheel1(base_t *base): base_(base){}

  void makeSubWheels() {
    CHECK(!!base_);
    CHECK(!secondWheel_ && !subsecondWheel_);
    secondWheel_.reset(second_wheel_t::createNew(base_));
    subsecondWheel_.reset(subsecond_wheel_t::createNew(base_));
  }
 public:
  static HTimerWheel1* createNew(base_t *base) {
    auto wheel = new HTimerWheel1(base);
    wheel->makeSubWheels();
    return wheel;
  }
  template<typename TCallable>
  void add(TCallable&& callable, uint64_t msecDelay, bool repeating) {
    if (msecDelay < subsecondWheel_->endOffsetMS()) {
      subsecondWheel_->add(
        std::forward<TCallable>(callable),
        msecDelay,
        repeating
      );
    } else {
      CHECK(msecDelay < secondWheel_->endOffsetMS());
      VoidCallback endCb = std::forward<TCallable>(callable);
      auto subOffset = msecDelay % secondWheel_->resolution();
      secondWheel_->add([endCb, this, subOffset]() {
        bool subRepeat = false;
        LOG(INFO) << "sub repeat";
        subsecondWheel_->add(endCb, subOffset, subRepeat);
      }, msecDelay, repeating);
    }
  }
  template<typename TCallable>
  void addRepeating(TCallable&& callable, uint64_t msecInterval) {
    bool repeating = true;
    add(std::forward<TCallable>(callable), msecInterval, repeating);
  }
  template<typename TCallable>
  void addOneShot(TCallable&& callable, uint64_t msecInterval) {
    bool repeating = false;
    add(std::forward<TCallable>(callable), msecInterval, repeating);
  }
  void start() {
    subsecondWheel_->start();
    secondWheel_->start();
  }
  void stop() {
    subsecondWheel_->stop();
    secondWheel_->stop();
  }
};

}}} // evs::events2::wheel
