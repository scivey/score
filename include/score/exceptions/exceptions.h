#pragma once
#include <stdexcept>
#include "score/exceptions/ScoreError.h"


namespace score { namespace exceptions {

SCORE_DECLARE_EXCEPTION(AssertionError, ScoreError);
SCORE_DECLARE_EXCEPTION(BaseError, ScoreError);

}} // score::exceptions
