#include "score/curl/detail/RealCurlAPI.h"
#include <glog/logging.h>

namespace score { namespace curl { namespace detail {

CURL* RealCurlAPI::easy_init() {
  return curl_easy_init();
}

void RealCurlAPI::easy_cleanup(CURL *handle) {
  curl_easy_cleanup(handle);
}

CURLM* RealCurlAPI::multi_init() {
  return curl_multi_init();
}

void RealCurlAPI::multi_cleanup(CURLM *handle) {
  curl_multi_cleanup(handle);
}

CURLMcode RealCurlAPI::multi_wait(CURLM *handle,
    struct curl_waitfd extraFds[], unsigned int extraNfds,
    int timeoutMs, int* numFds) {
  return curl_multi_wait(handle, extraFds, extraNfds, timeoutMs, numFds);
}

CURLMcode RealCurlAPI::multi_perform(CURLM *handle, int *runningHandles) {
  return curl_multi_perform(handle, runningHandles);
}

CURLMcode RealCurlAPI::multi_add_handle(CURLM *multi_handle, CURL *easy_handle) {
  return curl_multi_add_handle(multi_handle, easy_handle);
}

CURLMcode RealCurlAPI::multi_remove_handle(CURLM *multi_handle, CURL *easy_handle) {
  return curl_multi_remove_handle(multi_handle, easy_handle);
}

CURLMsg* RealCurlAPI::multi_info_read(CURLM *handle, int *nRemaining) {
  return curl_multi_info_read(handle, nRemaining);
}

}}} // score::curl::detail

