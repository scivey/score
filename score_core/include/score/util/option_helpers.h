#pragma once
#include "score/Optional.h"
#include <type_traits>

namespace score  { namespace util {

template<typename T>
struct is_option: std::false_type {
  using inner_type = T;
};

template<typename T>
struct is_option<score::Optional<T>>: std::true_type {
  using inner_type = T;
};


template<typename TIn, typename TCallable,
  typename = typename std::enable_if<
    !is_option<typename std::result_of<TCallable(TIn)>::type>::value,
    TIn
  >::type>
score::Optional<typename std::result_of<TCallable(TIn)>::type> doWithValue(
    score::Optional<TIn>&& prevOption, TCallable&& func) {
  using result_t = typename std::result_of<TCallable(TIn)>::type;
  using output_t = score::Optional<result_t>;
  output_t resultOption;
  if (prevOption.hasValue()) {
    resultOption.assign(func(std::move(prevOption.value())));
  }
  return resultOption;
}


template<typename TIn, typename TCallable,
  typename = typename std::enable_if<
    is_option<typename std::result_of<TCallable(TIn)>::type>::value,
    TIn
  >::type>
auto doWithValue(score::Optional<TIn>&& prevOption, TCallable&& func)
      -> decltype(std::declval<TCallable>()(std::declval<TIn>())) {
  using result_t = typename std::result_of<TCallable(TIn)>::type;
  if (prevOption.hasValue()) {
    return func(prevOption.value());
  }
  return result_t {};
}


}} // score::util
