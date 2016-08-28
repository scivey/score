#pragma once
#include "aliens/reactor/EpollReactor.h"

namespace aliens { namespace reactor {

template<typename T>
class ReflectedEpollTask : public EpollReactor::Task {
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
  ReflectedEpollTask(){}
  void onReadable() override {
    getParent()->onReadable();
  }
  void onWritable() override {
    getParent()->onWritable();
  }
  void onError() override {
    getParent()->onError();
  }
  int getFd() override {
    return getParent()->getFdNo();
  }
};

}} // aliens::reactor
