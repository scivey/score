#pragma once

#include <stdexcept>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <glog/logging.h>
#include <folly/Range.h>
#include "score/util/try_helpers.h"
#include "score/Try.h"
#include "score/Unit.h"
#include "score_curl/Easy.h"

namespace score { namespace curl {

class MultiHandle {
 protected:
  CURLM *multi_ {nullptr};
  SCORE_DISABLE_COPY_AND_ASSIGN(MultiHandle);
  MultiHandle(CURLM *multi): multi_(multi){}
 public:
  MultiHandle(){}
  MultiHandle(MultiHandle&& other): multi_(other.multi_) {
    other.multi_ = nullptr;
  }
  MultiHandle& operator=(MultiHandle&& other) {
    std::swap(multi_, other.multi_);
    return *this;
  }
  CURLM* get() {
    DCHECK(good());
    return multi_;
  }
  CURLM* operator->() {
    DCHECK(good());
    return multi_;
  }
  void addHandle(CURL *handle) {
    CHECK(curl_multi_add_handle(multi_, handle) == CURLM_OK);
  }
  bool good() const {
    return !!multi_;
  }
  operator bool() const {
    return good();
  }
  static MultiHandle create() {
    return takeOwnership(curl_multi_init());
  }
  static MultiHandle* createNew() {
    return new MultiHandle { create() };
  }
  static MultiHandle takeOwnership(CURLM *multi) {
    return MultiHandle(multi);
  }
  ~MultiHandle() {
    if (multi_) {
      curl_multi_cleanup(multi_);
      multi_ = nullptr;
    }
  }
};


class MultiError : public std::runtime_error {
 protected:
  CURLMcode curlCode_;
 public:
  MultiError(CURLMcode code)
    : std::runtime_error(curl_multi_strerror(code)), curlCode_(code) {}
  CURLMcode curlCode() const {
    return curlCode_;
  }
};


class Multi {
 protected:
  std::unique_ptr<MultiHandle> handle_ {nullptr};
  char errBuffer_[CURL_ERROR_SIZE];
 public:

  Multi(){}
  Multi(std::unique_ptr<MultiHandle>&& handle): handle_(std::move(handle)){}
  static Multi create() {
    return Multi(std::unique_ptr<MultiHandle> {
      MultiHandle::createNew()
    });
  }
  static Multi* createNew() {
    return new Multi(create());
  }
  CURLM* get() {
    return handle_->get();
  }
  bool good() const {
    return !!handle_;
  }
  operator bool() const {
    return good();
  }
  template<typename T>
  Try<Unit> setOptRaw(CURLMoption opt, T value) {
    CHECK(good());
    auto rc = curl_multi_setopt(get(), opt, value);
    if (rc != CURLM_OK) {
      return util::makeTryFailure<Unit, MultiError>(rc);
    }
    return util::makeTrySuccess<Unit>();
  }
  template<typename T>
  void setOpt(CURLMoption opt, T value) {
    setOptRaw(opt, value).throwIfFailed();
  }
  void addHandle(Easy& handle) {
    handle_->addHandle(handle.get());
  }
  void addHandle(Easy *handle) {
    handle_->addHandle(handle->get());
  }
  int perform() {
    int nRunning = 0;
    auto rc = curl_multi_perform(get(), &nRunning);
    CHECK(rc == CURLM_OK);
    return nRunning;
  }
  long getTimeout() {
    long result {0};
    auto rc = curl_multi_timeout(get(), &result);
    CHECK(rc == CURLM_OK);
    return result;
  }

  void bindCallbacks() {
    setOpt(CURLMOPT_SOCKETFUNCTION, Multi::curlSocketCallback);
    setOpt(CURLMOPT_SOCKETDATA, (void*) this);
    setOpt(CURLMOPT_TIMERFUNCTION, Multi::curlTimerCallback);
    setOpt(CURLMOPT_TIMERDATA, (void*) this);
  }


  using timer_cb_t = std::function<int(long)>;
 protected:
  timer_cb_t timerHandler_ {};
  int onCurlTimer(long tm) {
    if (timerHandler_) {
      return timerHandler_(tm);
    } else {
      LOG(INFO) << "no timer handler!";
      return 0;
    }
  }
 public:
  static int curlTimerCallback(CURLM *multi, long tm, void *data) {
    auto ctx = (Multi*) data;
    return ctx->onCurlTimer(tm);
  }

  template<typename TCallable>
  void setTimerHandler(TCallable&& callable) {
    timerHandler_ = std::move(callable);
  }
  template<typename TCallable>
  void setTimerHandler(const TCallable& callable) {
    timerHandler_ = callable;
  }


 public:
  using socket_cb_t = std::function<int(CURL*, curl_socket_t, int, void*)>;
 protected:
  socket_cb_t socketHandler_ {};
  int onCurlSocket(CURL *easy, curl_socket_t sockFd, int what, void *sockPtr) {
    if (socketHandler_) {
      return socketHandler_(easy, sockFd, what, sockPtr);
    } else {
      LOG(INFO) << "no socket handler!";
      return 0;
    }
  }

 public:
  static int curlSocketCallback(CURL *easy, curl_socket_t s, int what, void *userp, void *socketp) {
    auto ctx = (Multi*) userp;
    return ctx->onCurlSocket(easy, s, what, socketp);
  }

  template<typename TCallable>
  void setSocketHandler(TCallable&& callable) {
    socketHandler_ = std::move(callable);
  }
  template<typename TCallable>
  void setSocketHandler(const TCallable& callable) {
    socketHandler_ = callable;
  }
};


}} // score::curl
