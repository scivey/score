#pragma once

#include <string>
#include <glog/logging.h>
#include "score/mem/st_shared_ptr.h"
#include "score/func/Function.h"
#include "score_async/futures/st/traits.h"
#include "score_async/futures/st/STSharedState.h"
#include "score_async/futures/st/STFutureCore.h"
#include "score_async/futures/st/STPromise.h"

namespace score { namespace async { namespace futures { namespace st {


template<typename T>
class STFuture {
 public:
  using ttraits = st_traits<T>;
  using value_type = typename ttraits::value_type;
  using value_cb_t = typename ttraits::value_cb_t;
  using error_t = typename ttraits::error_t;
  using error_cb_t = typename ttraits::error_cb_t;
  using core_t = STFutureCore<T>;
  using core_ptr_t = score::mem::st_shared_ptr<core_t>;

 protected:
  core_ptr_t core_ {nullptr};
 public:
  STFuture(core_ptr_t core): core_(core){}
  void setDoneCallback(value_cb_t&& valCb) {
    CHECK(!!core_);
    core_->setValueCallback(std::forward<value_cb_t>(valCb));
  }



 protected:
  template<typename TOut>
  struct then_helper {
    using result_t = TOut;
    using cb_t = func::Function<T, result_t>;
    using cb_wrap_t = type_wrapper<cb_t>;
    using out_future = STFuture<TOut>;
    using promise_t = STPromise<TOut>;
    using promise_ptr_t = score::mem::st_shared_ptr<promise_t>;

    struct then_ctx: public std::enable_shared_from_this<then_ctx> {
      promise_ptr_t aPromise {nullptr};
      cb_t callback;
      then_ctx(promise_ptr_t &&prom, cb_t&& cb)
        : aPromise(std::move(prom)), callback(std::forward<cb_t>(cb)){}
    };

    static out_future bindThen(STFuture* parent, cb_t&& cb) {
      auto ctx = score::mem::make_st_shared<then_ctx>(
        score::mem::make_st_shared<STPromise<TOut>>(),
        std::forward<cb_t>(cb)
      );
      std::function<void(T)> parentCallback = [ctx](T result) {
        ctx->aPromise->setValue(ctx->callback(result));
      };
      parent->setDoneCallback(std::move(parentCallback));
      return ctx->aPromise->getFuture();
    }

    template<typename TCallable,
      typename = typename std::enable_if<
        !cb_wrap_t::template is_same<TCallable>::value,
        TCallable
      >::type>
    static out_future bindThen(STFuture* parent, TCallable&& callable) {
      cb_t bound {std::forward<TCallable>(callable)};
      return bindThen(parent, std::move(bound));
    }
  };

 public:


  template<typename U, typename X>
  struct resultOf {
    using unwrapped_func_t = typename std::remove_reference<U>::type;
    using type = decltype(std::declval<unwrapped_func_t>()(std::declval<X>()));
  };

  template<typename U>
  struct result_future_traits {
    using func_result_t = typename resultOf<U, T>::type;
    using type = typename Unit::Lift<func_result_t>::type;
  };

  template<typename U, typename X = T>
  struct returns_void {
    using result_type = typename resultOf<U, X>::type;
    static const bool value = std::is_same<result_type, void>::value;
  };

  template<typename TCallable,
    typename = typename std::enable_if<!returns_void<TCallable>::value, TCallable>::type
  >
  STFuture<typename result_future_traits<TCallable>::type> then(TCallable&& func) {
    using result_t = typename result_future_traits<TCallable>::type;
    return then_helper<result_t>::bindThen(
      this, std::forward<TCallable>(func)
    );
  }

  template<typename TCallable,
    typename = typename std::enable_if<returns_void<TCallable>::value, TCallable>::type
  >
  STFuture<Unit> then(TCallable&& callable) {
    using helper_t = then_helper<Unit>;
    using cb_t = typename helper_t::cb_t;
    std::function<void(T)> toWrap { std::forward<TCallable>(callable) };
    cb_t func = [toWrap](T result) {
      toWrap(result);
      return Unit{};
    };
    return then_helper<Unit>::bindThen(this, std::move(func));
  }

  void orCatch(error_cb_t&& errCb) {
    CHECK(!!core_);
    core_->setErrorCallback(std::forward<error_cb_t>(errCb));
  }
};


template<typename T>
struct isFuture<STFuture<T>>: std::true_type {};

}}}} // score::async::futures::st
