#pragma once

#include <chrono>
#include <sys/types.h>
#include <sys/timerfd.h>

namespace score { namespace reactor {

namespace detail {
void convertTime(timespec*, std::chrono::milliseconds);
}

class TimerSettings {
 public:
  using duration_type = std::chrono::milliseconds;
 protected:
  duration_type initialDelay_ {0};
  duration_type intervalDuration_ {0};
  itimerspec timerSpec_;
  void updateTimerSpec();
public:
  TimerSettings();
  TimerSettings(duration_type initial, duration_type interval);
  itimerspec* getTimerSpec();
};

}} // score::reactor
