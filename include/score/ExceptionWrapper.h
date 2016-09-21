#pragma once
#include "score/vendored/folly/ExceptionWrapper.h"

namespace score {

using ExceptionWrapper = score::vendored::folly::exception_wrapper;

template<typename T, typename... Args>
ExceptionWrapper makeExceptionWrapper(Args&& ...args) {
  return score::vendored::folly::make_exception_wrapper<T, Args...>(
    std::forward<Args>(args)...
  );
}

} // score

