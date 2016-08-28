#pragma once

#include <sys/timerfd.h>
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/TimerSettings.h"


namespace aliens { namespace reactor {

class TimerFd {
 public:
  class EpollTask : public EpollReactor::Task {
   protected:
    TimerFd* parent_ {nullptr};
    EpollTask(){}
    void setParent(TimerFd *parent);
    friend class TimerFd;
    TimerFd* getParent() const;
   public:
    void onReadable() override;
    void onWritable() override;
    void onError() override;
    int getFd() override;
  };

  friend class EpollTask;

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
  void triggerRead();
 public:
  EpollTask *getEpollTask();
  int getFdNo() const;
  static TimerFd create(const TimerSettings&, EventHandler *handler);
  static std::shared_ptr<TimerFd> createShared(const TimerSettings&, EventHandler *handler);
  void stop();
};

}} // aliens::reactor