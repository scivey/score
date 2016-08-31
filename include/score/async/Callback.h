#pragma once
#include <functional>
#include <array>

namespace score { namespace async {

template<typename T>
struct Callback {
  using func = std::function<void(T)>;
  func func_;
  Callback(func &&func)
    : func_(std::forward<func>(func)) {}

  Callback(const func &func): func_(func){}

  template<typename TCallable>
  Callback(TCallable &&callable)
    : func_(std::forward<TCallable>(callable)) {}

  template<typename TCallable>
  Callback(const TCallable &callable): func_(callable) {}

  void invoke(const T &value) {
    func_(value);
  }
  void invoke(T &&value) {
    func_(std::forward<T>(value));
  }

  void operator()(const T &value) {
    invoke(value);
  }

  void operator()(T &&value) {
    invoke(std::forward<T>(value));
  }
};

}}