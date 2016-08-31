#pragma once
#include "score/exceptions/exceptions.h"

#define SCORE_CHECK_SYSCALL(expr) \
  do { \
    if ((expr) < 0) { \
      throw score::exceptions::SystemError(errno); \
    } \
  } while (0); \

#define SCORE_CHECK_SYSCALL2(expr, msg) \
  do { \
    if ((expr) < 0) { \
      throw score::exceptions::SystemError::fromErrno(errno, msg); \
    } \
  } while (0); \

#define ACHECK(expr, args...) \
  do { \
    if (!(expr)) { \
      throw score::exceptions::AssertionError( \
        "assertion failed: " #expr "" #args \
      ); \
    } \
  } while (0)

#ifndef NDEBUG
  #define ADCHECK(expr, args...) ACHECK(expr, msg)
#else
  #define ADCHECK(expr, args...)
#endif

