#pragma once
#include "score/exceptions/exceptions.h"

#define SCHECK(expr, args...) \
  do { \
    if (!(expr)) { \
      throw score::exceptions::AssertionError( \
        "assertion failed: " #expr "" #args \
      ); \
    } \
  } while (0)

#ifndef NDEBUG
  #define SDCHECK(expr, args...) SCHECK(expr, msg)
#else
  #define SDCHECK(expr, args...)
#endif

