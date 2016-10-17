#pragma once

#include "evs/events2/VoidCallback.h"

namespace evs { namespace events2 {

class JustOnce {
 protected:
  bool ran_ {false};
  VoidCallback func_;
 public:
  JustOnce(VoidCallback&& func): func_(func){}
  bool run() {
    if (!ran_) {
      ran_ = true;
      func_();
      return true;
    }
    return false;
  }
  void operator()() {
    run();
  }
};

}} // evs::events2

