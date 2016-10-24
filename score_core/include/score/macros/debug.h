#pragma once
#include <glog/logging.h>
#include "score/demangle.h"
#include "score/format.h"

#define SCORE_LOG_ADDR0() do { \
    LOG(INFO) << score::format( \
      "my address: {} [instance of {}]", \
      ((uintptr_t) this), \
      score::demangle(typeid(decltype(this))) \
    ); \
  } while (0)
