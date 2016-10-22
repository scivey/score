#pragma once

#include "score_async/vendored/concurrentqueue/concurrentqueue.h"

namespace score { namespace async { namespace queues {

template<typename T>
using MPMCQueue = score::async::vendored::moodycamel::ConcurrentQueue<T>;

}}} // score::async::queues