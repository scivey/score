#pragma once
#include <curl/curl.h>
#include <vector>
#include <sstream>
#include <string>
#include "score/macros.h"

namespace score { namespace curl {

// sfinae wrapper around CURL
template<typename TCurlAPI>
class CurlEasyHandle {
 public:
  using curl_api_policy = TCurlAPI;
  using handle_type = CURL;
 protected:
  std::string url_;
  handle_type *handlePtr_ {nullptr};
  CurlEasyHandle(){}
  CurlEasyHandle(const CurlEasyHandle&) = delete;
  CurlEasyHandle& operator=(const CurlEasyHandle&) = delete;
  void maybeCleanup() {
    if (handlePtr_) {
      curl_api_policy::easy_cleanup(handlePtr_);
      handlePtr_ = nullptr;
    }
  }

 public:
  template<typename T>
  void setOpt(CURLoption option, T data) {
    curl_api_policy::easy_setopt_checked(handlePtr_, option, data);
  }

  CurlEasyHandle(CurlEasyHandle &&other)
    : handlePtr_(other.handlePtr_) {
    other.handlePtr_ = nullptr;
  }
  CurlEasyHandle& operator=(CurlEasyHandle &&other) {
    if (handlePtr_ == other.handlePtr_) {
      other.handlePtr_ = nullptr;
      return *this;
    }
    auto swapPtr = handlePtr_;
    handlePtr_ = other.handlePtr_;
    other.handlePtr_ = swapPtr;
    return *this;
  }
  CurlEasyHandle& operator=(CURL *easyPtr) {
    if (handlePtr_ == easyPtr) {
      return *this;
    }
    maybeCleanup();
    handlePtr_ = easyPtr;
    return *this;
  }
  static CurlEasyHandle create() {
    CurlEasyHandle instance;
    instance.handlePtr_ = curl_api_policy::easy_init();
    return instance;
  }
  static CurlEasyHandle create(CURL *easyHandle) {
    CurlEasyHandle instance;
    instance.handlePtr_ = easyHandle;
    return instance;
  }
  void setURL(const std::string &url) {
    url_ = url;
    setOpt(CURLOPT_URL, url_.c_str());
  }
  CURL* getCurlHandle() {
    SDCHECK(valid());
    return handlePtr_;
  }
  bool valid() const {
    return !!handlePtr_;
  }
  ~CurlEasyHandle() {
    maybeCleanup();
  }
};

}} // score::curl