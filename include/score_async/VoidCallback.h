#pragma once

#include <functional>
#include <glog/logging.h>

namespace score { namespace async {

class VoidCallback {
 public:
  using func_t = std::function<void()>;
  typedef void (*func_ptr_t)();

 protected:
  func_t func_ {};
 public:

  VoidCallback(){}

  template<typename TCallable,
    typename std::enable_if<!std::is_same<TCallable, func_ptr_t>::value, TCallable>::type>
  VoidCallback(TCallable&& callable)
    : func_(std::forward<TCallable>(callable)) {}

  template<typename TCallable>
  VoidCallback(const TCallable& callable)
    : func_(callable) {}

  template<typename TCallable>
  VoidCallback& operator=(const TCallable& callable) {
    func_ = callable;
    return *this;
  }

  template<typename TCallable>
  VoidCallback& operator=(TCallable&& callable) {
    func_ = std::move(callable);
    return *this;
  }

  bool good() const {
    return !!func_;
  }

  operator bool() const {
    return good();
  }

  void operator()() {
    DCHECK(good());
    func_();
  }
};


}} // score::async