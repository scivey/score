#pragma once
#include "score/reactor/EpollReactor.h"
#include "score/ParentHaving.h"


namespace score { namespace reactor {

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

}} // score::reactor
