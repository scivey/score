#pragma once
#include "score/exceptions/exceptions.h"

namespace score { namespace url {

class URLError: public score::exceptions::BaseError {
 public:
  template<typename T>
  URLError(const T& msg): score::exceptions::BaseError(msg){}
};

class URLParseError: public URLError {
 public:
  template<typename T>
  URLParseError(const T& msg): URLError(msg){}
};

}} // score::url
