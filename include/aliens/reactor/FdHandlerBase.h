#pragma once

#include <memory>
#include "aliens/reactor/ReflectedEpollTask.h"

namespace aliens { namespace reactor {

template<typename T>
class FdHandlerBase {
 public:
  friend class ReflectedEpollTask<T>;
  using EpollTask = ReflectedEpollTask<T>;
 protected:
  FileDescriptor fd_;
  EpollTask epollTask_;
  T* getThis() {
    return (T*) this;
  }
 public:
  void onReadable() {
    getThis()->triggerReadable();
  }
  void onWritable() {
    getThis()->triggerWritable();
  }
  void onError() {
    getThis()->triggerError();
  }
  EpollTask* getEpollTask() {
    return &epollTask_;
  }
  FdHandlerBase(FileDescriptor &&fd)
    : fd_(std::forward<FileDescriptor>(fd)) {}
};


}} // aliens::reactor
