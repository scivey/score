#pragma once

#include <memory>
#include <glog/logging.h>
#include "aliens/reactor/ReflectedEpollTask.h"
#include "aliens/reactor/EpollReactor.h"
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
    LOG(INFO) << "triggerWritable";
    getThis()->onWritable();
  }
  void triggerError() {
    LOG(INFO) << "triggerError";
    getThis()->onError();
  }
  EpollTask* getEpollTask() {
    return &epollTask_;
  }
  FdHandlerBase(FileDescriptor &&fd)
    : fd_(std::forward<FileDescriptor>(fd)) {
    epollTask_.setParent(getThis());
  }
  FileDescriptor& getFileDescriptor() {
    return fd_;
  }
  int getFdNo() const {
    LOG(INFO) << "here..";
    return fd_.getFdNo();
  }
  void stop() {
    fd_.close();
  }
};


}} // aliens::reactor
