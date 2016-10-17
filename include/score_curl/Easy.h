#pragma once

#include <stdexcept>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <glog/logging.h>
#include <folly/Range.h>
#include "score/util/try_helpers.h"
#include "score_async/UnaryCallback.h"
#include "score/Try.h"
#include "score/Unit.h"

namespace score { namespace curl {


class EasyHandle {
 protected:
  CURL *easy_ {nullptr};
  SCORE_DISABLE_COPY_AND_ASSIGN(EasyHandle);
  EasyHandle(CURL *easy): easy_(easy){}
 public:
  EasyHandle(){}
  EasyHandle(EasyHandle&& other): easy_(other.easy_) {
    other.easy_ = nullptr;
  }
  EasyHandle& operator=(EasyHandle&& other) {
    std::swap(easy_, other.easy_);
    return *this;
  }
  CURL* get() {
    DCHECK(good());
    return easy_;
  }
  CURL* operator->() {
    DCHECK(good());
    return easy_;
  }
  bool good() const {
    return !!easy_;
  }
  operator bool() const {
    return good();
  }
  static EasyHandle create() {
    return takeOwnership(curl_easy_init());
  }
  static EasyHandle* createNew() {
    return new EasyHandle { create() };
  }
  static EasyHandle takeOwnership(CURL *easy) {
    return EasyHandle(easy);
  }
  ~EasyHandle() {
    if (easy_) {
      curl_easy_cleanup(easy_);
      easy_ = nullptr;
    }
  }
};

class EasyError : public std::runtime_error {
 protected:
  CURLcode curlCode_;
 public:
  EasyError(CURLcode code)
    : std::runtime_error(curl_easy_strerror(code)), curlCode_(code) {}
  CURLcode curlCode() const {
    return curlCode_;
  }
};


class Easy {
 public:
  using done_cb_t = async::UnaryCallback<std::string>;
 protected:
  done_cb_t doneCallback_;
  std::unique_ptr<EasyHandle> handle_ {nullptr};
  std::ostringstream bodyBuffer_;
  char errBuffer_[CURL_ERROR_SIZE];
  std::string url_;
 public:
  Easy(){}
  Easy(std::unique_ptr<EasyHandle>&& handle): handle_(std::move(handle)){}
  static Easy create() {
    return Easy(std::unique_ptr<EasyHandle> {
      EasyHandle::createNew()
    });
  }
  template<typename TCallable>
  void setDoneCallback(TCallable&& callable) {
    doneCallback_ = callable;
  }
  static Easy* createNew() {
    return new Easy(create());
  }
  CURL* get() {
    return handle_->get();
  }
  const std::string& getURL() const {
    return url_;
  }
  bool good() const {
    return !!handle_;
  }
  operator bool() const {
    return good();
  }
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
  Try<Unit> perform() {
    auto rc = curl_easy_perform(get());
    if (rc != CURLE_OK) {
      return util::makeTryFailure<Unit, EasyError>(rc);
    }
    return util::makeTrySuccess<Unit>();
  }
  size_t onWrite(void *ptr, size_t sz, size_t nMembers) {
    if (sz && nMembers) {
      folly::StringPiece piece {(char*) ptr, nMembers};
      bodyBuffer_ << piece;
    }
    size_t total = sz * nMembers;
    return total;
  }
  static size_t curlWriteCallback(void *ptr, size_t sz, size_t nm, void *data) {
    auto ctx = (Easy*) data;
    return ctx->onWrite(ptr, sz, nm);
  }
  int onProgress(double total, double now, double ult, double uln) {
    return 0;
  }
  static size_t curlProgressCallback(void *data, double a, double b, double c, double d) {
    auto ctx = (Easy*) data;
    return ctx->onProgress(a, b, c, d);
  }
  void bindCallbacks() {
    setOpt(CURLOPT_PROGRESSFUNCTION, Easy::curlProgressCallback);
    setOpt(CURLOPT_PROGRESSDATA, (void*) this);
    setOpt(CURLOPT_WRITEFUNCTION, Easy::curlWriteCallback);
    setOpt(CURLOPT_PRIVATE, (void*) this);
    setOpt(CURLOPT_WRITEDATA, (void*) this);
    setOpt(CURLOPT_ERRORBUFFER, (char*) errBuffer_);
    setOpt(CURLOPT_VERBOSE, 0L);
  }
  void setURL(const std::string& url) {
    url_ = url;
    setOpt(CURLOPT_URL, url_.c_str());
  }
  std::ostringstream& getBodyBuffer() {
    return bodyBuffer_;
  }
  curl_socket_t getFD() {
    curl_socket_t sock {0};
    auto rc = curl_easy_getinfo(get(), CURLINFO_LASTSOCKET, &sock);
    CHECK(rc == CURLE_OK);
    return sock;
  }
  static Easy* fromOwnedEasyHandle(CURL *ptr) {
    uintptr_t privPointer {0};
    auto rc = curl_easy_getinfo(ptr, CURLINFO_PRIVATE, &privPointer);
    CHECK(rc == CURLE_OK);
    CHECK(privPointer != 0);
    return (Easy*) privPointer;
  }
  void onFinished() {
    CHECK(doneCallback_);
    doneCallback_(bodyBuffer_.str());
  }
};

}} // score::curl
