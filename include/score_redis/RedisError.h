#pragma once

#include <stdexcept>
#include "score/exceptions/ScoreError.h"

namespace score { namespace redis {

#define X SCORE_DECLARE_EXCEPTION

X(RedisError, score::exceptions::ScoreError);
X(RedisIOError, RedisError);
X(RedisProtocolError, RedisError);
X(RedisEOFError, RedisError);
X(RedisTypeError, RedisError);
X(AlreadySubscribedError, RedisError);
X(SubscriptionError, RedisError);

#undef X


}} // fredis::redis
