#pragma once
#include <thread>
#include <memory>
#include "score/macros.h"

namespace score { namespace folly_util {

template<typename TEventBase>
class EBThread {
  std::unique_ptr<std::thread> thread_ {nullptr};
  std::unique_ptr<TEventBase> base_ {nullptr};
  EBThread(){}
 public:
  static std::shared_ptr<EBThread> createShared() {
    std::shared_ptr<EBThread> instance {new EBThread};
    instance->base_.reset(new TEventBase);
    return instance;
  }
  void start() {
    SCHECK(!thread_);
    thread_.reset(new std::thread([this]() {
      base_->loopForever();
    }));
  }
  void stop() {
    SCHECK(!!thread_);
  }
  void join() {
    SCHECK(!!thread_);
    thread_->join();
  }
  TEventBase* getBase() {
    return base_.get();
  }
};

}} // score::folly_util

