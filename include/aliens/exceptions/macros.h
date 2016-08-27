#pragma once
#include "aliens/exceptions/exceptions.h"

#define ALIENS_CHECK_SYSCALL(expr) \
  do { \
    if ((expr) < 0) { \
      throw aliens::exceptions::SystemError(errno); \
    } \
  } while (0); \

#define ALIENS_CHECK_SYSCALL2(expr, msg) \
  do { \
    if ((expr) < 0) { \
      throw aliens::exceptions::SystemError::fromErrno(errno, msg); \
    } \
  } while (0); \
