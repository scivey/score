#pragma once

#include <sys/eventfd.h>
#include "aliens/reactor/EpollReactor.h"

namespace aliens { namespace reactor {

class EventFd {
 public:
  class EpollTask : public EpollReactor::Task {
   protected:
    EventFd* parent_ {nullptr};
    EpollTask(){}
    void setParent(EventFd *parent);
    friend class EventFd;
    EventFd* getParent() const;
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
  EpollTask epollTask_;
  EventHandler *handler_ {nullptr};
  void triggerRead();
  EventFd(FileDescriptor &&desc, EventHandler *handler);
 public:
  EpollTask *getEpollTask();
  int getFdNo() const;
  static EventFd create(EventHandler*);
  static std::shared_ptr<EventFd> createShared(EventHandler*);
  void stop();
};

}} // aliens::reactor