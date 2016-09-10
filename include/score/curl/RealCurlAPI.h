#pragma once
#include <curl/curl.h>
#include "score/curl/CurlError.h"

namespace score { namespace curl {

struct RealCurlAPI {
  static CURL* easy_init();
  static void easy_cleanup(CURL*);
  static CURLM* multi_init();
  static void multi_cleanup(CURLM*);
  static CURLMcode multi_wait(
    CURLM*, struct curl_waitfd extraFds[],
    unsigned int extraNfds, int timeoutMs, int* numFds
  );
  static CURLMcode multi_perform(CURLM*, int* running);
  static CURLMcode multi_add_handle(CURLM*, CURL*);

  template<typename T>
  static CURLcode easy_setopt(CURL* handle, CURLoption option,
      const T& value) {
    return curl_easy_setopt(handle, option, value);
  }

  template<typename T>
  static CURLcode easy_setopt_checked(CURL* handle, CURLoption option,
      const T& value) {
    auto rc = curl_easy_setopt(handle, option, value);
    if (rc != CURLE_OK) {
      throw CurlEasyError::fromCode(rc);
    }
    return rc;
  }

  template<typename T>
  static CURLMcode multi_setopt(CURLM *handle, CURLMoption option,
      const T& value) {
    return curl_multi_setopt(handle, option, value);
  }

  template<typename T>
  static CURLMcode multi_setopt_checked(CURLM *handle, CURLMoption option,
      const T& value) {
    auto rc = curl_multi_setopt(handle, option, value);
    if (rc != CURLM_OK) {
      throw CurlMError::fromCode(rc);
    }
    return rc;
  }

  static CURLMcode multi_remove_handle(CURLM*, CURL*);

  template<typename TValue>
  static CURLMcode multi_socket_action(CURLM *handle, int opt, TValue val, int* numPtr) {
    auto rc = curl_multi_socket_action(handle, opt, val, numPtr);
    if (rc != CURLM_OK) {
      throw CurlMError::fromCode(rc);
    }
    return rc;
  }

  static CURLMsg* multi_info_read(CURLM *handle, int *nRemaining);
};

}} // score::curl
