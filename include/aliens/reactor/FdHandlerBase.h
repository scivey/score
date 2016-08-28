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
    : fd_(std::forward<FileDescriptor>(fd)) {
    epollTask_.setParent(getThis());
  }
  int getFdNo() const {
    return fd_.getFdNo();
  }
  void stop() {
    fd_.close();
  }
};


}} // aliens::reactor
