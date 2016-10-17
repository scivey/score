#pragma once
#include "score/macros.h"

namespace score {

class UniqueToken {
 protected:
  bool value_ {false};
  SCORE_DISABLE_COPY_AND_ASSIGN(UniqueToken);
  UniqueToken(bool value): value_(value){}
 public:
  UniqueToken(){}
  void mark() {
    value_ = true;
  }
  void clear() {
    value_ = false;
  }
  UniqueToken(UniqueToken&& other): value_(other.value_) {
    other.value_ = false;
  }
  UniqueToken& operator=(UniqueToken&& other) {
    std::swap(value_, other.value_);
    return *this;
  }
  bool good() const {
    return value_;
  }
  operator bool() const {
    return good();
  }
};

} // score


