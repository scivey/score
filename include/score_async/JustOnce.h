#pragma once

#include "score/func/Function.h"

namespace score { namespace async {

class JustOnce {
 public:
  using cb_t = func::Function<void>;
 protected:
  bool ran_ {false};

  cb_t func_;
 public:
  JustOnce(cb_t&& func): func_(std::forward<cb_t>(func)) {}
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

