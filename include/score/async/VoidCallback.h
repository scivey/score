#pragma once
#include <functional>
#include <array>
#include "score/Maybe.h"

namespace score { namespace async {

class VoidCallback {
 public:
  using void_func = std::function<void()>;

 protected:
  Maybe<void_func> func_;

 public:
  VoidCallback(){}
  VoidCallback(void_func &&func): func_(std::move(func)) {}
  VoidCallback(const void_func &func): func_(func){}

  template<typename TCallable>
  VoidCallback(TCallable &&callable): func_(std::move(callable)){}

  template<typename TCallable>
  VoidCallback(const TCallable &callable): func_(callable) {}

  void invoke() {
    if (func_.hasValue()) {
      func_.value()();
    }
  }

  void operator()() {
    invoke();
  }
};

}}