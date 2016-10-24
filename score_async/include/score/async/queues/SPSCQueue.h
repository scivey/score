#pragma once

#include "score/async/vendored/readerwriterqueue/readerwriterqueue.h"

namespace score { namespace async { namespace queues {

template<typename T>
using SPSCQueue = score::async::vendored::moodycamel::ReaderWriterQueue<T>;

}}} // score::async::queues