#pragma once

#include <sys/timerfd.h>
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/TimerSettings.h"
#include "aliens/reactor/ReflectedEpollTask.h"

namespace aliens { namespace reactor {

class TimerFd {
 public:
  friend class ReflectedEpollTask<TimerFd>;
  using EpollTask = ReflectedEpollTask<TimerFd>;

  class EventHandler {
   public:
    virtual void onTick() = 0;
    virtual ~EventHandler() = default;
  };
 protected:
  FileDescriptor fd_;
  TimerSettings settings_;
  EpollTask epollTask_;
  EventHandler *handler_ {nullptr};

  TimerFd(FileDescriptor &&desc, EventHandler *handler);
  void onReadable();
  void onWritable();
  void onError();
 public:
  EpollTask *getEpollTask();
  int getFdNo() const;
  static TimerFd create(const TimerSettings&, EventHandler *handler);
  static std::shared_ptr<TimerFd> createShared(const TimerSettings&, EventHandler *handler);
  void stop();
};

}} // aliens::reactor