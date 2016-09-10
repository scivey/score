#pragma once
#include <curl/curl.h>
#include "score/exceptions/exceptions.h"

namespace score { namespace curl {

class CurlError : public score::exceptions::BaseError {
 public:
  template<typename T>
  CurlError(const T& msg): score::exceptions::BaseError(msg){}
};

class CurlMultiError: public CurlError {
 public:
  template<typename T>
  CurlMultiError(const T&msg): CurlError(msg){}
  static CurlMultiError fromCode(CURLMcode code);
};

class CurlEasyError: public CurlError {
 public:
  template<typename T>
  CurlEasyError(const T&msg): CurlError(msg){}
  static CurlEasyError fromCode(CURLcode code);
};

}}