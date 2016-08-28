#include "aliens/reactor/TimerSettings.h"
#include <cstring>

namespace aliens { namespace reactor {

namespace detail {
void convertTime(timespec *target, std::chrono::milliseconds msec) {
  size_t sec = msec.count() / 1000;
  target->tv_sec = sec;
  size_t remainingMsec = msec.count() - (sec * 1000);
  if (remainingMsec) {
    target->tv_nsec = remainingMsec * 1000000;
  } else {
    target->tv_nsec = 0;
  }
}
} // detail

void TimerSettings::updateTimerSpec() {
  detail::convertTime(&timerSpec_.it_value, initialDelay_);
  detail::convertTime(&timerSpec_.it_interval, intervalDuration_);
}
TimerSettings::TimerSettings() {
  memset(&timerSpec_, '\0', sizeof(timerSpec_));
}

TimerSettings::TimerSettings(
  typename TimerSettings::duration_type init,
  typename TimerSettings::duration_type interval)
  : initialDelay_(init), intervalDuration_(interval) {
  updateTimerSpec();
}
itimerspec* TimerSettings::getTimerSpec() {
  return &timerSpec_;
}

}} // aliens::reactor
