#pragma once

#include "score_async/VoidCallback.h"

namespace score { namespace async {

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

}} // score::async

