#pragma once
#include <atomic>
#include <functional>
#include "score/func/Function.h"
#include "score/UniqueToken.h"

namespace score {

class ScopeGuard {
 public:
  using void_func_t = func::Function<void>;
 protected:
  void_func_t func_;
  UniqueToken token_;
  bool dismissed_ {false};
 public:
  ScopeGuard(void_func_t &&func);
  ScopeGuard(ScopeGuard&&);
  ScopeGuard& operator=(ScopeGuard&&);
  void dismiss();
  ~ScopeGuard();
};

ScopeGuard makeGuard(typename ScopeGuard::void_func_t&& func);

} // score
