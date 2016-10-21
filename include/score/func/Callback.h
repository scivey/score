#pragma once

#include <functional>
#include <type_traits>
#include <glog/logging.h>
#include "score/Optional.h"

namespace score { namespace func {

template<typename T>
class Callback {
 public:
  using arg_type = T;
  using func_type = std::function<void(T)>;
  using func_option_t = score::Optional<func_type>;
 protected:
  func_option_t func_;
 public:
  Callback(){}
  bool good() const {
    return !!func_;
  }
  explicit operator bool() const {
    return good();
  }
  Callback(const func_type& fn): func_(fn){}
  Callback(func_type&& fn): func_(std::move(fn)){}

  Callback(const Callback& other): func_(other.func_){}
  Callback(Callback&& other): func_(std::move(other.func_)) {}

  Callback& operator=(const Callback& other) {
    func_.assign(other.func_);
    return *this;
  }
  Callback& operator=(Callback&& other) {
    std::swap(func_, other.func_);
    return *this;
  }

  Callback& operator=(const func_type& fn) {
    func_.assign(fn);
    return *this;
  }

  Callback& operator=(func_type&& fn) {
    func_.assign(std::move(fn));
    return *this;
  }

  template<typename U>
  struct is_func_type {
    static const bool value = std::is_same<
        typename std::remove_reference<U>::type,
        func_type
      >::value || std::is_same<
        typename std::remove_reference<U>::type,
        Callback
      >::value;
  };


  template<typename TCallable,
    typename = typename std::enable_if<
      !is_func_type<TCallable>::value,
      TCallable
    >::type
  >
  Callback(TCallable&& callable) {
    func_.assign(std::move(callable));
  }

  template<typename TCallable,
    typename = typename std::enable_if<
      !is_func_type<TCallable>::value,
      TCallable
    >::type
  >
  Callback(const TCallable& callable): func_(callable){}


  template<typename TCallable,
    typename = typename std::enable_if<
      !is_func_type<TCallable>::value,
      TCallable
    >::type
  >
  Callback& operator=(TCallable&& callable) {
    func_.assign(std::move(callable));
    return *this;
  }

  template<typename TCallable,
    typename = typename std::enable_if<
      !is_func_type<TCallable>::value,
      TCallable
    >::type
  >
  Callback& operator=(const TCallable& callable) {
    func_.assign(callable);
    return *this;
  }

  void call(T arg) {
    CHECK(good());
    func_.value()(arg);
  }

  void operator()(T arg) {
    call(arg);
  }
};

}} // score::func
