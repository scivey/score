#include "aliens/reactor/TimerFd.h"
#include <atomic>
#include <memory>
#include <cstdio>
#include <glog/logging.h>
#include "aliens/macros.h"

using aliens::posix::FileDescriptor;

namespace aliens { namespace reactor {


TimerFd::TimerFd(FileDescriptor &&desc, TimerFd::EventHandler *handler)
  : FdHandlerBase<TimerFd>(std::forward<FileDescriptor>(desc)),
    handler_(handler) {}

void TimerFd::onReadable() {
  uint64_t nTimeouts {0};
  ADCHECK(8 == read(getFdNo(), &nTimeouts, sizeof(uint64_t)));
  ADCHECK(!!handler_);

  // nTimeouts should almost always be 1.
  // are there edge cases?
  for (size_t i = 0; i < nTimeouts; i++) {
    handler_->onTick();
  }
}

void TimerFd::onWritable() {
  LOG(INFO) << "TimerFd::onWritable";
}

void TimerFd::onError() {
  LOG(INFO) << "TimerFd::onError";
}

TimerFd TimerFd::create(const TimerSettings& settings, TimerFd::EventHandler *handler) {
  int fd = timerfd_create(
    CLOCK_MONOTONIC,
    TFD_NONBLOCK | TFD_CLOEXEC
  );
  ALIENS_CHECK_SYSCALL2(fd, "TimerFd::create()");
  TimerFd instance(FileDescriptor::fromIntExcept(fd), handler);
  instance.settings_ = settings;
  itimerspec previousSettings;
  itimerspec *desiredSettings = instance.settings_.getTimerSpec();

  const int kRelativeTimerFlag = 0;
  ALIENS_CHECK_SYSCALL2(
    timerfd_settime(
      instance.getFdNo(),
      kRelativeTimerFlag,
      desiredSettings,
      &previousSettings
    ),
    "timerfd_settime()"
  );
  return instance;
}

TimerFd* TimerFd::createPtr(
    const TimerSettings& settings, TimerFd::EventHandler *handler) {
  return new TimerFd(create(
    settings, handler
  ));
}


}} // aliens::reactor
