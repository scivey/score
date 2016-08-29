#pragma once
#include <memory>
#include "aliens/Maybe.h"
#include "aliens/macros.h"
#include "aliens/async/Callback.h"

namespace aliens { namespace futures {

template<typename T>
class Promise;

template<typename T>
class FutureCore: public std::enable_shared_from_this<FutureCore<T>> {
 protected:
  std::shared_ptr<Promise<T>> parent_ {nullptr};
  Maybe<async::Callback> onResult_;
 public:
  Future(std::shared_ptr<Promise<T>> parent)
    : parent_(parent) {}

  template<typename TCallable>
  auto then(TCallable &&callable) -> Future<std::decltype(callable(std::declval<T>()))> {
    CHECK(!onResult_.hasValue());
    using result_type = std::decltype(callable(std::declval<T>()));
    using MoveWrapper = MoveWrapper<decltype(callable)>;
    auto wrapper = makeMoveWrapper(std::move(callable));
    auto self = shared_from_this();
    auto promise = std::make_shared<Promise<result_type>>();
    onResult_.assign([this, self, wrapper, promise](T result) {
      MoveWrapper unwrapped = wrapper;
      TCallable callable = unwrapped.move();
      auto mapped = callable(std::move(result));
      promise->resolve(std::move(mapped));
    });
    return promise->getFuture();
  }
};


template<typename T>
class Promise : public std::enable_shared_from_this<Promise<T>> {
 public:
  using value_type = T;
 protected:
  Maybe<T> value_;
  std::shared_ptr<Future<T>> future_ {nullptr};
 public:
  Promise() {

  }
};

}} // aliens::futures