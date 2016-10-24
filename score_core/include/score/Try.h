#pragma once

#include "score/vendored/folly/Try.h"

namespace score {

template<typename T>
using Try = score::vendored::folly::Try<T>;

}