#pragma once

#include <sys/eventfd.h>
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/FdHandlerBase.h"

namespace aliens { namespace reactor {

class EventFd : public FdHandlerBase<EventFd> {
 public:
  class EventHandler {
   public:
    virtual void onEvent(uint64_t) = 0;
    virtual ~EventHandler() = default;
  };

 protected:
  EventHandler *handler_ {nullptr};
  EventFd(posix::FileDescriptor &&desc, EventHandler *handler);

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