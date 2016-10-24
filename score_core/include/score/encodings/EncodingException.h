#pragma once
#include "score/exceptions/exceptions.h"

namespace score { namespace encodings {

class EncodingError: public exceptions::BaseError {
 public:
  template<typename T>
  EncodingError(const T& msg): exceptions::BaseError(msg){}
};

class BadEncodingName: public EncodingError {
 public:
  template<typename T>
  BadEncodingName(const T& msg): EncodingError(msg) {}
};

}} // score::encodings