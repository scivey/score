#pragma once

#include <memory>
#include <glog/logging.h>
#include "score/reactor/ReflectedEpollTask.h"
#include "score/reactor/EpollReactor.h"
namespace score { namespace reactor {

template<typename T>
class FdHandlerBase {
 public:
  friend class ReflectedEpollTask<T>;
  using EpollTask = ReflectedEpollTask<T>;
 protected:
  posix::FileDescriptor fd_;
  EpollTask epollTask_;
  T* getThis() {
    return (T*) this;
  }
 public:
  EpollReactor* getReactor() {
    auto taskReactor = epollTask_.getReactor();
    CHECK(!!taskReactor);
    return taskReactor;
  }
  void triggerReadable() {
    LOG(INFO) << "triggerReadable";
    getThis()->onReadable();
  }
  void triggerWritable() {
    VLOG(100) << "triggerWritable";
    getThis()->onWritable();
  }
  void triggerError() {
    VLOG(100) << "triggerError";
    getThis()->onError();
  }
  EpollTask* getEpollTask() {
    return &epollTask_;
  }
  FdHandlerBase(posix::FileDescriptor &&fd)
    : fd_(std::forward<posix::FileDescriptor>(fd)) {
    epollTask_.setParent(getThis());
  }
  posix::FileDescriptor& getFileDescriptor() {
    return fd_;
  }
  int getFdNo() const {
    return fd_.getFdNo();
  }
  void stop() {
    fd_.close();
  }
};


}} // score::reactor
