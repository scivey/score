#pragma once

#include <memory>
#include "aliens/reactor/ReflectedFdHandlerTask.h"

namespace aliens { namespace reactor {

template<typename T>
class FdHandlerBase {
 public:
  friend class ReflectedFdHandlerTask<T>;
  using EpollTask = ReflectedFdHandlerTask<T>;
 protected:
  FileDescriptor fd_;
  EpollTask epollTask_;
  T* getThis() {
    return (T*) this;
  }
 public:
  void triggerReadable() {
    getThis()->onReadable();
  }
  void triggerWritable() {
    getThis()->onWritable();
  }
  void triggerError() {
    getThis()->onError();
  }
  EpollTask* getEpollTask() {
    return &epollTask_;
  }
  FdHandlerBase(FileDescriptor &&fd)
    : fd_(std::forward<FileDescriptor>(fd)) {}
};


}} // aliens::reactor
