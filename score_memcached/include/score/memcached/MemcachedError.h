#pragma once

#include "score/exceptions/ScoreError.h"

namespace score { namespace memcached {

SCORE_DECLARE_EXCEPTION(MemcachedError, score::exceptions::ScoreError);
SCORE_DECLARE_EXCEPTION(ConfigurationError, MemcachedError);
SCORE_DECLARE_EXCEPTION(ConnectionError, MemcachedError);
SCORE_DECLARE_EXCEPTION(AlreadyConnected, ConnectionError);
SCORE_DECLARE_EXCEPTION(ProtocolError, MemcachedError);


}} // score::memcached
