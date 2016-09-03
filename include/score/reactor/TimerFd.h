#pragma once

#include <sys/timerfd.h>
#include <glog/logging.h>
#include "score/reactor/EpollReactor.h"
#include "score/reactor/TimerSettings.h"
#include "score/reactor/FdHandlerBase.h"
#include "score/reactor/ReflectedEpollTask.h"
#include "score/PointerFactory.h"
#include "score/ParentHaving.h"

namespace score { namespace reactor {

class TimerFd : public FdHandlerBase<TimerFd> {
 public:

  class EventHandler: public ParentHaving<TimerFd> {
   public:
    virtual void onTick() = 0;
    virtual ~EventHandler() = default;
    virtual void stop();
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

  // ~TimerFd() {
  //   LOG(INFO) << "~TimerFd()";
  // }
};

}} // score::reactor