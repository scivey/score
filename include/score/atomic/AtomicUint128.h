#pragma once
#include "score/atomic/IntrinsicAtomic.h"

namespace score { namespace atomic {

using AtomicUint128 = IntrinsicAtomic<__uint128_t>;


}} // score::atomic
