#pragma once
#include <functional>
#include <array>
#include <stdexcept>
#include <exception>
#include "aliens/Maybe.h"

namespace aliens { namespace async {

class ErrBack {
 public:
  using exception_type = std::exception;
  using except_option = aliens::Maybe<exception_type>;
  using err_func = std::function<void(const except_option&)>;

 protected:
  Maybe<err_func> func_;

 public:
  ErrBack(){}
  ErrBack(err_func &&func): func_(std::move(func)) {}
  ErrBack(const err_func &func): func_(func){}

  template<typename TCallable>
  ErrBack(TCallable &&callable) {
    err_func fn = std::move(callable);
    func_.assign(std::move(fn));
  }

  template<typename TCallable>
  ErrBack(const TCallable &callable): func_(callable) {}

  void invoke(const except_option &ex) {
    if (func_.hasValue()) {
      func_.value()(ex);
    }
  }

  void invoke() {
    if (func_.hasValue()) {
      except_option nothing;
      func_.value()(nothing);
    }
  }

  void operator()(const except_option &ex) {
    invoke(ex);
  }

  void operator()(const std::exception &ex) {
    invoke(except_option(ex));
  }

  void operator()() {
    invoke();
  }
};

}}