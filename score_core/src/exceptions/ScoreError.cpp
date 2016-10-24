#include "score/exceptions/ScoreError.h"

namespace score { namespace exceptions {

ScoreError::ScoreError(const char *msg): std::runtime_error(msg) {}

}} // evs::exceptions
