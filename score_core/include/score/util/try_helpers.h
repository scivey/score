#pragma once

#include <functional>
#include "score/Try.h"
#include "score/ExceptionWrapper.h"
#include "score/exceptions/PosixError.h"
#include "score/format.h"
#include "score/demangle.h"
#include <type_traits>


namespace score { namespace util {

template<typename TValue, typename TErr>
score::Try<TValue> makeTryFailureFromErrno(int err, const std::string& msg) {
  return score::Try<TValue> {
    score::make_exception_wrapper<TErr>(exceptions::formatErrno(err, msg))
  };
}

template<typename TValue, typename TErr, typename ... TErrorArgs>
score::Try<TValue> makeTryFailure(TErrorArgs&&... errArgs) {
  return score::Try<TValue> {
    score::make_exception_wrapper<TErr>(std::forward<TErrorArgs>(errArgs)...)
  };
}

template<typename TValue>
score::Try<TValue> makeTryFailure(score::exception_wrapper&& exWrap) {
  return score::Try<TValue> {
    std::forward<score::exception_wrapper>(exWrap)
  };
}

template<typename TValue, typename TErr>
score::Try<TValue> makeTryFailure(const TErr& err) {
  return makeTryFailure<TValue, TErr>(err.what());
}

template<typename TValue, typename TErr1, typename TErr2,
  typename = typename std::enable_if<std::is_base_of<std::exception, TErr2>::value, TErr2>::type,
  typename = typename std::enable_if<!std::is_same<TErr1, TErr2>::value, TErr2>::type>
score::Try<TValue> makeTryFailure(TValue, const TErr2& otherErr) {
  return makeTryFailure<TValue, TErr1>(
    score::sformat("{}: '{}'", score::demangle(typeid(TErr2)), otherErr.what())
  );
}

template<typename TValue, typename ... TValueArgs>
typename score::Try<TValue> makeTrySuccess(TValueArgs&&... valArgs) {
  return score::Try<TValue> {
    TValue {
      std::forward<TValueArgs>(valArgs)...
    }
  };
}

template<typename TValue>
score::Try<TValue> makeTrySuccess() {
  return score::Try<TValue>{TValue{}};
}


template<typename T>
struct is_try: std::false_type {
  using inner_type = T;
};

template<typename T>
struct is_try<score::Try<T>>: std::true_type {
  using inner_type = T;
};


template<typename TIn, typename TCallable,
  typename = typename std::enable_if<
    !is_try<typename std::result_of<TCallable(TIn)>::type>::value,
    TIn
  >::type>
score::Try<typename std::result_of<TCallable(TIn)>::type> doWithValue(
    score::Try<TIn>&& prevTry, TCallable&& func) {
  using result_t = typename std::result_of<TCallable(TIn)>::type;
  using output_t = score::Try<result_t>;
  if (prevTry.hasValue()) {
    return output_t {
      func(std::move(prevTry.value()))
    };
  }
  return output_t {
    std::move(prevTry.exception())
  };
}


template<typename TIn, typename TCallable,
  typename = typename std::enable_if<
    is_try<typename std::result_of<TCallable(TIn)>::type>::value,
    TIn
  >::type>
auto doWithValue(score::Try<TIn>&& prevTry, TCallable&& func)
      -> decltype(std::declval<TCallable>()(std::declval<TIn>())) {
  using result_t = typename std::result_of<TCallable(TIn)>::type;
  if (prevTry.hasValue()) {
    return func(prevTry.value());
  }
  return result_t {
    std::move(prevTry.exception())
  };
}

}} // score::util
