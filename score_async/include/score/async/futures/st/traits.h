#pragma once
#include <type_traits>
#include "score/Unit.h"
#include "score/ExceptionWrapper.h"
#include "score/Optional.h"
#include "score/func/Function.h"


namespace score { namespace async { namespace futures { namespace st {

template<typename T>
struct st_traits {
  using value_type = typename Unit::Lift<T>::type;
  using error_t = score::ExceptionWrapper;
  using value_cb_t = func::Function<void, value_type>;
  using error_cb_t = func::Function<void, error_t>;
  using value_option_t = score::Optional<value_type>;
  using error_option_t = score::Optional<error_t>;
};

template<typename T>
struct type_wrapper {
  using type = T;


  template<typename U>
  struct is_same {
    static const bool value = std::is_same<T, U>::value;
  };

  template<typename U>
  struct not_same {
    static const bool value = !is_same<U>::value;
  };
};


template<typename T>
struct isFuture: std::false_type {};


}}}} // score::async::futures::st
