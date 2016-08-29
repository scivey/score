#pragma once

#include <sys/eventfd.h>
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/FdHandlerBase.h"

namespace aliens { namespace reactor {

class SignalFd: public FdHandlerBase<SignalFd> {
 public:
  class EventHandler {
   public:
    virtual void onSignal(uint32_t) = 0;
    virtual ~EventHandler() = default;
  };

 protected:
  EventHandler *handler_ {nullptr};
  SignalFd(posix::FileDescriptor &&desc, EventHandler *handler);
 public:
  void onReadable();
  void onWritable();
  void onError();
  static SignalFd create(EventHandler*);
  static std::shared_ptr<SignalFd> createShared(EventHandler*);
};

}} // aliens::reactor