#pragma once

#include <sys/eventfd.h>
#include "score/reactor/EpollReactor.h"
#include "score/reactor/FdHandlerBase.h"
#include "score/PointerFactory.h"

namespace score { namespace reactor {

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
  static SignalFd* createPtr(EventHandler*);
  friend class PointerFactory<SignalFd>;
 public:
  using Factory = PointerFactory<SignalFd>;
  void onReadable();
  void onWritable();
  void onError();
  static SignalFd create(EventHandler*);
};

}} // score::reactor