#pragma once
#include <functional>
#include <tuple>

namespace score { namespace func {


// primary template.
template<class T>
struct callable_traits : callable_traits<decltype(&T::operator())> {
};

// partial specialization for function type
template<class R, class... Args>
struct callable_traits<R(Args...)> {
    using result_type = R;
    using argument_types = std::tuple<Args...>;
};

// partial specialization for function pointer
template<class R, class... Args>
struct callable_traits<R (*)(Args...)> {
    using result_type = R;
    using argument_types = std::tuple<Args...>;
};

// partial specialization for std::function
template<class R, class... Args>
struct callable_traits<std::function<R(Args...)>> {
    using result_type = R;
    using argument_types = std::tuple<Args...>;
};

// partial specialization for pointer-to-member-function (i.e., operator()'s)
template<class T, class R, class... Args>
struct callable_traits<R (T::*)(Args...)> {
    using result_type = R;
    using argument_types = std::tuple<Args...>;
};

template<class T, class R, class... Args>
struct callable_traits<R (T::*)(Args...) const> {
    using result_type = R;
    using argument_types = std::tuple<Args...>;
};

// additional cv-qualifier and ref-qualifier combinations omitted
// sprinkle with C-style variadics if desired

template<typename T, size_t Idx>
using nth_arg_type = typename std::tuple_element<
    Idx, typename callable_traits<T>::argument_types
  >::type;

}} // score::func

