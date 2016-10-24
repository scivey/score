#pragma once

#include <stdexcept>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <glog/logging.h>
#include <folly/Range.h>
#include "score/util/try_helpers.h"
#include "score/func/Function.h"
#include "score/Try.h"
#include "score/Unit.h"
#include "score/curl/EasyHandle.h"
#include "score/curl/EasyError.h"

namespace score { namespace curl {

class Easy {
 public:
  using done_cb_t = func::Function<void, std::string>;
 protected:
  done_cb_t doneCallback_;
  std::unique_ptr<EasyHandle> handle_ {nullptr};
  std::ostringstream bodyBuffer_;
  char errBuffer_[CURL_ERROR_SIZE];
  std::string url_;
 public:
  Easy();
  Easy(std::unique_ptr<EasyHandle>&& handle);
  static Easy create();

  template<typename TCallable>
  void setDoneCallback(TCallable&& callable) {
    doneCallback_ = callable;
  }
  static Easy* createNew();
  CURL* get();
  const std::string& getURL() const;
  bool good() const;
  operator bool() const;

  template<typename T>
  Try<Unit> setOptRaw(CURLoption opt, T value) {
    CHECK(good());
    auto rc = curl_easy_setopt(get(), opt, value);
    if (rc != CURLE_OK) {
      return util::makeTryFailure<Unit, EasyError>(rc);
    }
    return util::makeTrySuccess<Unit>();
  }

  template<typename T>
  void setOpt(CURLoption opt, T value) {
    setOptRaw(opt, value).throwIfFailed();
  }

  Try<Unit> perform();

  size_t onWrite(void *ptr, size_t sz, size_t nMembers);

  static size_t curlWriteCallback(void *ptr, size_t sz, size_t nm, void *data);
  int onProgress(double total, double now, double ult, double uln);
  static size_t curlProgressCallback(void *data, double a, double b, double c, double d);
  void bindCallbacks();
  void setURL(const std::string& url);
  std::ostringstream& getBodyBuffer();
  curl_socket_t getFD();
  static Easy* fromOwnedEasyHandle(CURL *ptr);
  void onFinished();
};

}} // score::curl
