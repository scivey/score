#pragma once

#include <sys/timerfd.h>
#include "score/reactor/EpollReactor.h"
#include "score/reactor/TimerSettings.h"
#include "score/reactor/FdHandlerBase.h"
#include "score/reactor/ReflectedEpollTask.h"
#include "score/PointerFactory.h"
namespace score { namespace reactor {

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

}} // score::reactor