#pragma once

#include <stdexcept>
#include "score/exceptions/ScoreError.h"

namespace score { namespace redis {

#define X SCORE_DECLARE_EXCEPTION

X(RedisError, score::exceptions::ScoreError);
X(AlreadyConnected, RedisError);
X(NotConnected, RedisError);
X(RedisIOError, RedisError);
X(RedisEOFError, RedisIOError);
X(RedisTypeError, RedisError);
X(RedisProtocolError, RedisError);
X(SubscriptionError, RedisProtocolError);
X(AlreadySubscribedError, SubscriptionError);

#undef X


}} // fredis::redis
