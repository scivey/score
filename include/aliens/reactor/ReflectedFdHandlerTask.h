#pragma once
#include "aliens/reactor/EpollReactor.h"

namespace aliens { namespace reactor {

template<typename T>
class ReflectedFdHandlerTask : public EpollReactor::Task {
 public:
  using parent_type = T;

 protected:
  parent_type* parent_ {nullptr};
 public:
  void setParent(parent_type *parent) {
    parent_ = parent;
  }
  parent_type* getParent() const {
    return parent_;
  }
  ReflectedFdHandlerTask(){}
  void onReadable() override {
    getParent()->triggerReadable();
  }
  void onWritable() override {
    getParent()->triggerWritable();
  }
  void onError() override {
    getParent()->triggerError();
  }
  int getFd() override {
    return getParent()->getFdNo();
  }
};

}} // aliens::reactor
