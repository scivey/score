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
#include "score/curl/Easy.h"
#include "score/curl/MultiHandle.h"
#include "score/curl/MultiError.h"

namespace score { namespace curl {

class Multi {
 protected:
  std::unique_ptr<MultiHandle> handle_ {nullptr};
  char errBuffer_[CURL_ERROR_SIZE];
 public:

  Multi();
  Multi(std::unique_ptr<MultiHandle>&& handle);
  static Multi create();
  static Multi* createNew();
  CURLM* get();
  bool good() const;
  operator bool() const;

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

  void addHandle(Easy& handle);
  void addHandle(Easy *handle);
  int perform();
  long getTimeout();
  void bindCallbacks();

  using timer_cb_t = std::function<int(long)>;
 protected:
  timer_cb_t timerHandler_ {};
  int onCurlTimer(long tm);

 public:
  static int curlTimerCallback(CURLM *multi, long tm, void *data);

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
  int onCurlSocket(CURL *easy, curl_socket_t sockFd, int what, void *sockPtr);

 public:
  static int curlSocketCallback(CURL *easy, curl_socket_t s, int what, void *userp, void *socketp);

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
