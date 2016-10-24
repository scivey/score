#pragma once

#include "score/vendored/folly/Optional.h"

namespace score {

template<typename T>
using Optional = score::vendored::folly::Optional<T>;

} // score
