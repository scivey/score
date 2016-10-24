#pragma once

#include "score/async/vendored/concurrentqueue/blockingconcurrentqueue.h"

namespace score { namespace async { namespace queues {

template<typename T>
using MPMCQueue = score::async::vendored::moodycamel::BlockingConcurrentQueue<T>;

}}} // score::async::queues