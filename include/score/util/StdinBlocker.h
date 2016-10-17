#pragma once
#include "score/logging.h"
#include <sys/types.h>

namespace score { namespace util {

class StdinBlocker {
 protected:
  char buff_[1024];
 public:
  StdinBlocker(){}
  void run() {
    char* buffPtr = (char*) buff_;
    size_t buffLen = sizeof(buff_);
    SCORE_INFO("Waiting for stdin: pid={}", getpid());
    getline(&buffPtr, &buffLen, stdin);
    SCORE_INFO("Stopped blocking.");
  }
};

}} // score::util
