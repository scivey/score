#pragma once

#include <functional>
#include <glog/logging.h>
#include <type_traits>

namespace score { namespace async {

template<typename T>
class UnaryCallback {
 public:
  using func_t = std::function<void(T)>;
  typedef void (*func_ptr_t)(T);
 protected:
  func_t func_ {};
 public:

  UnaryCallback(){}

  UnaryCallback(func_ptr_t ptr): func_(ptr) {}

  template<typename TCallable,
    typename std::enable_if<!std::is_same<TCallable, func_ptr_t>::value, TCallable>::type>
  UnaryCallback(TCallable&& callable)
    : func_(std::forward<TCallable>(callable)) {}


  template<typename TCallable>
  UnaryCallback(const TCallable& callable)
    : func_(callable) {}

  template<typename TCallable>
  UnaryCallback& operator=(const TCallable& callable) {
    func_ = callable;
    return *this;
  }

  UnaryCallback& operator=(func_ptr_t ptr) {
    func_ = ptr;
    return *this;
  }

  template<typename TCallable,
    typename std::enable_if<!std::is_same<TCallable, func_ptr_t>::value, TCallable>::type>
  UnaryCallback& operator=(TCallable&& callable) {
    func_ = std::move(callable);
    return *this;
  }

  bool good() const {
    return !!func_;
  }

  operator bool() const {
    return good();
  }

  // template<typename M = T, std::enable_if<!std::is_pointer<M>::value, M>::type>
  void call(T val) {
    DCHECK(good());
    func_(std::forward<T>(val));
  }

  // template<typename M = T, std::enable_if<std::is_pointer<M>::value, M>::type>

  void operator()(T val) {
    call(std::forward<T>(val));
  }
};

}} // score::async