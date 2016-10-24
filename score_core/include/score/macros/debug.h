#pragma once
#include <glog/logging.h>
#include <folly/Format.h>
#include "score/demangle.h"

#define SCORE_LOG_ADDR0() do { \
    LOG(INFO) << folly::format( \
      "my address: {} [instance of {}]", \
      ((uintptr_t) this), \
      score::demangle(typeid(decltype(this))) \
    ); \
  } while (0)
