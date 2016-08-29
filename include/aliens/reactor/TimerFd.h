#pragma once

#include <sys/timerfd.h>
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/TimerSettings.h"
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/reactor/ReflectedEpollTask.h"
#include "aliens/PointerFactory.h"
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
  static TimerFd* createPtr(const TimerSettings&, EventHandler *handler);
  friend class PointerFactory<TimerFd>;
 public:
  using Factory = PointerFactory<TimerFd>;
  void onReadable();
  void onWritable();
  void onError();
  static TimerFd create(const TimerSettings&, EventHandler *handler);
};

}} // aliens::reactor