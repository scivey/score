#pragma once

#include <vector>
#include <memory>
#include <glog/logging.h>

#include "evs/events2/EvBase.h"
#include "evs/events2/SigEvent.h"

namespace evs { namespace events2 {


class SignalHandlerRegistry {
 protected:
  EvBase *base_ {nullptr};
  std::vector<std::shared_ptr<SigEvent>> handlers_;
 public:
  SignalHandlerRegistry(EvBase *base): base_(base) {}
  template<typename TCallable>
  void addHandler(int signo, TCallable&& callable) {
    DCHECK(!!base_);
    std::shared_ptr<SigEvent> evt {
      SigEvent::createNewSignalEvent(base_, signo)
    };
    evt->setHandler(std::forward<TCallable>(callable));
    evt->add();
    handlers_.push_back(std::move(evt));
  }
  static SignalHandlerRegistry* createNew(EvBase *base) {
    return new SignalHandlerRegistry(base);
  }
};


}} // evs::events2