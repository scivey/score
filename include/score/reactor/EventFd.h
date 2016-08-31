#pragma once

#include <sys/eventfd.h>
#include "score/PointerFactory.h"
#include "score/reactor/EpollReactor.h"
#include "score/reactor/FdHandlerBase.h"

namespace score { namespace reactor {

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
  static EventFd* createPtr(EventHandler*);
  friend class PointerFactory<EventFd>;
 public:
  using Factory = PointerFactory<EventFd>;
  static EventFd create(EventHandler*);
  void onReadable();
  void onWritable();
  void onError();
  void stop();
  void write(uint64_t);
};

}} // score::reactor