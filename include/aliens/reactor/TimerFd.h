#pragma once

#include <sys/timerfd.h>
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/TimerSettings.h"
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/reactor/ReflectedEpollTask.h"

namespace aliens { namespace reactor {

class TimerFd : public FdHandlerBase<TimerFd> {
 public:

  class EventHandler {
   public:
    virtual void onTick() = 0;
    virtual ~EventHandler() = default;
  };
 protected:
  TimerSettings settings_;
  EventHandler *handler_ {nullptr};

  TimerFd(posix::FileDescriptor &&desc, EventHandler *handler);
 public:
  void onReadable();
  void onWritable();
  void onError();
  static TimerFd create(const TimerSettings&, EventHandler *handler);
  static std::shared_ptr<TimerFd> createShared(const TimerSettings&, EventHandler *handler);
};

}} // aliens::reactor