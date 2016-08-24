#pragma once
#include <atomic>
#include <functional>

namespace asioz {

class ScopeGuard {
 public:
  using void_func = std::function<void()>;
 protected:
  void_func func_;
  std::atomic<bool> dismissed_ {false};
 public:
  ScopeGuard(void_func &&func);
  void dismiss();
  ~ScopeGuard();

  template<typename TCallable>
  ScopeGuard(TCallable &&callable)
    : func_(std::move(callable)) {}

};

} // asioz
