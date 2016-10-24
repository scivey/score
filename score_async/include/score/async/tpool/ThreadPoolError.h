#pragma once
#include "score/exceptions/ScoreError.h"
#include "score/async/EventError.h"

namespace score { namespace async { namespace tpool {

SCORE_DECLARE_EXCEPTION(ThreadPoolError, EventError);
SCORE_DECLARE_EXCEPTION(NotRunning, ThreadPoolError);
SCORE_DECLARE_EXCEPTION(InvalidSettings, ThreadPoolError);
SCORE_DECLARE_EXCEPTION(AlreadyRunning, ThreadPoolError);

SCORE_DECLARE_EXCEPTION(TaskError, ThreadPoolError);
SCORE_DECLARE_EXCEPTION(InvalidTask, TaskError);
SCORE_DECLARE_EXCEPTION(TaskThrewException, TaskError);
SCORE_DECLARE_EXCEPTION(CouldntSendResult, TaskError);

}}} // score::async::tpool
