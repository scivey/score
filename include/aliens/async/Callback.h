#pragma once
#include <functional>
#include <array>

namespace aliens { namespace async {

template<typename T>
struct Callback {
  using func = std::function<void(T)>;
  func func_;
  Callback(func &&func): func_(std::move(func)) {}
  Callback(const func &func): func_(func){}

  template<typename TCallable>
  Callback(TCallable &&callable): func_(std::move(callable)){}

  template<typename TCallable>
  Callback(const TCallable &callable): func_(callable) {}

  void invoke(T value) {
    func_(value);
  }
};

}}