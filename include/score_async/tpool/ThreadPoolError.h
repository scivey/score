#pragma once
#include "score/exceptions/ScoreError.h"
#include "score_async/EventError.h"

namespace score { namespace async { namespace tpool {

SCORE_DECLARE_EXCEPTION(ThreadPoolError, EventError);
SCORE_DECLARE_EXCEPTION(NotRunning, ThreadPoolError);
SCORE_DECLARE_EXCEPTION(InvalidSettings, ThreadPoolError);
SCORE_DECLARE_EXCEPTION(AlreadyRunning, ThreadPoolError);

}}} // score::async::tpool
