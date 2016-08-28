#pragma once

#include <sys/eventfd.h>
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/FdHandlerBase.h"
// #include "aliens/reactor/ReflectedEpollTask.h"

namespace aliens { namespace reactor {

class EventFd : public FdHandlerBase<EventFd> {
 public:
  // friend class ReflectedEpollTask<EventFd>;
  // using EpollTask = ReflectedEpollTask<EventFd>;

  class EventHandler {
   public:
    virtual void onEvent(uint64_t) = 0;
    virtual ~EventHandler() = default;
  };

 protected:
  // FileDescriptor fd_;
  // EpollTask epollTask_;
  EventHandler *handler_ {nullptr};
  EventFd(FileDescriptor &&desc, EventHandler *handler);

 public:
  void onReadable();
  void onWritable();
  void onError();
  static EventFd create(EventHandler*);
  static std::shared_ptr<EventFd> createShared(EventHandler*);
  void stop();
  void write(uint64_t);
};

}} // aliens::reactor