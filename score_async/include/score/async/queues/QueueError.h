#pragma once
#include "score/async/EventError.h"
#include "score/exceptions/ScoreError.h"

namespace score { namespace async { namespace queues {

#define X SCORE_DECLARE_EXCEPTION
X(QueueError, EventError);
X(QueueWriteError, QueueError);
X(PartialWriteError, QueueWriteError);
X(QueueFull, QueueWriteError);
#undef X

}}} // score::async::queues