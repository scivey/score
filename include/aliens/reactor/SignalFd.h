#pragma once

#include <sys/eventfd.h>
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/ReflectedEpollTask.h"

namespace aliens { namespace reactor {

class SignalFd {
 public:
  friend class ReflectedEpollTask<SignalFd>;
  using EpollTask = ReflectedEpollTask<SignalFd>;

  class EventHandler {
   public:
    virtual void onSignal(uint32_t) = 0;
    virtual ~EventHandler() = default;
  };

 protected:
  FileDescriptor fd_;
  EpollTask epollTask_;
  EventHandler *handler_ {nullptr};
  SignalFd(FileDescriptor &&desc, EventHandler *handler);
  void onReadable();
  void onWritable();
  void onError();
 public:
  EpollTask *getEpollTask();
  int getFdNo() const;
  static SignalFd create(EventHandler*);
  static std::shared_ptr<SignalFd> createShared(EventHandler*);
  void stop();
};

}} // aliens::reactor