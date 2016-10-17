#pragma once
#include "score/exceptions/ScoreError.h"
#include <folly/Format.h>
#include <errno.h>

namespace score { namespace exceptions {

SCORE_DECLARE_EXCEPTION(PosixError, ScoreError);

}}


#define SCORE_THROW_POSIX_ERROR(callExpr) \
    throw evs::exceptions::PosixError(folly::sformat( \
      "PosixError: [{}] '{}' from call '{}'", \
      errno, strerror(errno), #callExpr \
    ))

#define SCORE_CHECK_POSIX_CALL(rc, callExpr) do { \
    if ((rc) < 0) { \
      SCORE_THROW_POSIX_ERROR(callExpr); \
    } \
  while (0)


#define SCORE_MAKE_POSIX_CALL(expr) do { \
    auto rc = (expr); \
    SCORE_CHECK_POSIX_CALL(rc, expr); \
  } while (0)


#define SCORE_CHECK_POSIX_NOTNULL(rc, expr) do { \
    if (!(rc)) { \
      SCORE_THROW_POSIX_ERROR(#expr); \
    } \
  } while (0)
