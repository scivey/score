#pragma once
#include <functional>
#include <array>

namespace aliens { namespace async {

struct VoidCallback {
  using void_func = std::function<void()>;
  void_func func_;
  VoidCallback(void_func &&func): func_(std::move(func)) {}
  VoidCallback(const void_func &func): func_(func){}

  template<typename TCallable>
  VoidCallback(TCallable &&callable): func_(std::move(callable)){}

  template<typename TCallable>
  VoidCallback(const TCallable &callable): func_(callable) {}

  void invoke() {
    func_();
  }
};

}}