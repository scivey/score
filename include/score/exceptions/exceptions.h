#pragma once
#include <stdexcept>
#include "score/exceptions/ScoreError.h"


namespace score { namespace exceptions {

SCORE_DECLARE_EXCEPTION(AssertionError, ScoreError);
SCORE_DECLARE_EXCEPTION(BaseError, ScoreError);

class SystemError: public BaseError {
 protected:
  int errno_ {0};
 public:
  SystemError(int err, const std::string &msg);
  SystemError(int err);
  int getErrno() const;
  static SystemError fromErrno(int err, const std::string &msg);
};



}} // score::exceptions
