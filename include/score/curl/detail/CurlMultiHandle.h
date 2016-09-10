#pragma once
#include <vector>
#include <chrono>
#include <thread>
#include <curl/curl.h>
#include "score/curl/detail/CurlEasyHandle.h"
#include "score/macros.h"

namespace score { namespace curl { namespace detail {

// sfinae wrapper around CURLM

template<typename TCurlAPI>
class CurlMultiHandle {
 public:
  using curl_api_policy = TCurlAPI;
  using handle_type = CURLM;
  using easy_handle_type = CurlEasyHandle<TCurlAPI>;
 protected:
  handle_type *handlePtr_ {nullptr};
  CurlMultiHandle(){}
  CurlMultiHandle(const CurlMultiHandle&) = delete;
  CurlMultiHandle& operator=(const CurlMultiHandle&) = delete;
  void maybeCleanup() {
    if (handlePtr_) {
      curl_api_policy::multi_cleanup(handlePtr_);
      handlePtr_ = nullptr;
    }
  }
 public:
  CurlMultiHandle(CurlMultiHandle &&other)
    : handlePtr_(other.handlePtr_) {
    other.handlePtr_ = nullptr;
  }
  CurlMultiHandle& operator=(CurlMultiHandle &&other) {
    auto swapPtr = handlePtr_;
    handlePtr_ = other.handlePtr_;
    other.handlePtr_ = swapPtr;
    return *this;
  }
  static CurlMultiHandle create() {
    CurlMultiHandle instance;
    instance.handlePtr_ = curl_api_policy::multi_init();
    return instance;
  }

  void addEasyHandle(easy_handle_type &easyHandle) {
    SDCHECK(valid());
    SDCHECK(easyHandle.valid());
    curl_api_policy::multi_add_handle(handlePtr_, easyHandle.getCurlHandle());
  }

  CURLM* getCurlHandle() {
    SDCHECK(valid());
    return handlePtr_;
  }

  template<typename T>
  void setOpt(CURLMoption opt, T data) {
    SDCHECK(valid());
    auto rc = curl_api_policy::multi_setopt(handlePtr_, opt, data);
    SDCHECK(rc == CURLM_OK);
  }

  void removeEasyHandle(CURL *handle) {
    SDCHECK(valid());
    curl_api_policy::multi_remove_handle(getCurlHandle(), handle);
  }

  bool valid() const {
    return !!handlePtr_;
  }

  ~CurlMultiHandle() {
    maybeCleanup();
  }
};

}}} // score::curl::detail