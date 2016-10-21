#pragma once

#include <functional>
#include <glog/logging.h>
#include <type_traits>
#include "score/Optional.h"

namespace score { namespace func {

template<typename TIn, typename TOut>
class Function {
 public:
  using arg_type = TIn;
  using input_type = TIn;
  using output_type = TOut;
  using func_type = std::function<output_type(input_type)>;
  using func_option_t = score::Optional<func_type>;
 protected:
  func_option_t func_;
 public:
  Function(){}
  bool good() const {
    return !!func_;
  }
  explicit operator bool() const {
    return good();
  }
  Function(const func_type& fn): func_(fn){}
  Function(func_type&& fn): func_(std::move(fn)){}

  Function(const Function& other): func_(other.func_){}
  Function(Function&& other): func_(std::move(other.func_)) {}

  Function& operator=(const Function& other) {
    func_.assign(other.func_);
    return *this;
  }
  Function& operator=(Function&& other) {
    std::swap(func_, other.func_);
    return *this;
  }

  Function& operator=(const func_type& fn) {
    func_.assign(fn);
    return *this;
  }

  Function& operator=(func_type&& fn) {
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
        Function
      >::value;
  };


  template<typename TCallable,
    typename = typename std::enable_if<
      !is_func_type<TCallable>::value,
      TCallable
    >::type
  >
  Function(TCallable&& callable) {
    func_type func {std::move(callable)};
    func_.assign(std::move(func));
  }

  template<typename TCallable,
    typename = typename std::enable_if<
      !is_func_type<TCallable>::value,
      TCallable
    >::type
  >
  Function(const TCallable& callable) {
    func_type func {callable};
    func_.assign(std::move(func));
  }


  template<typename TCallable,
    typename = typename std::enable_if<
      !is_func_type<TCallable>::value,
      TCallable
    >::type
  >
  Function& operator=(TCallable&& callable) {
    func_type fn {std::move(callable)};
    func_.assign(std::move(fn));
    return *this;
  }

  template<typename TCallable,
    typename = typename std::enable_if<
      !is_func_type<TCallable>::value,
      TCallable
    >::type
  >
  Function& operator=(const TCallable& callable) {
    func_type fn {callable};
    func_.assign(std::move(fn));
    return *this;
  }

  TOut call(TIn arg) {
    DCHECK(good());
    return func_.value()(arg);
  }

  TOut operator()(TIn arg) {
    return call(arg);
  }
};

}} // score::func