#pragma once
#include "aliens/reactor/EpollReactor.h"
#include "aliens/ParentHaving.h"


namespace aliens { namespace reactor {

template<typename T>
class ReflectedEpollTask : public ParentHaving<T>,
                           public EpollReactor::Task {
 public:
  ReflectedEpollTask(){}
  void onReadable() override {
    this->getParent()->triggerReadable();
  }
  void onWritable() override {
    this->getParent()->triggerWritable();
  }
  void onError() override {
    this->getParent()->triggerError();
  }
  int getFd() override {
    return this->getParent()->getFdNo();
  }
};

}} // aliens::reactor
