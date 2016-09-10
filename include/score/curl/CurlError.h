#pragma once
#include <curl/curl.h>
#include "score/exceptions/exceptions.h"

namespace score { namespace curl {

class CurlBaseError : public score::exceptions::BaseError {
 public:
  template<typename T>
  CurlBaseError(const T& msg): score::exceptions::BaseError(msg){}
};

class CurlMError: public CurlBaseError {
 public:
  template<typename T>
  CurlMError(const T&msg): CurlBaseError(msg){}
  static CurlMError fromCode(CURLMcode code);
};

class CurlEasyError: public CurlBaseError {
 public:
  template<typename T>
  CurlEasyError(const T&msg): CurlBaseError(msg){}
  static CurlEasyError fromCode(CURLcode code);
};

}}