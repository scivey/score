#pragma once

#include <memory>
#include <string>
#include "score_async/EventContext.h"
#include "score_curl/Easy.h"
#include "score_curl/Multi.h"
#include "score/util/misc.h"

namespace score { namespace curl {

class EventCurler {
 public:
  using ctx_t = score::async::EventContext;
  using easy_t = score::curl::Easy;
  using multi_t = score::curl::Multi;
  using multi_ptr_t = std::unique_ptr<multi_t>;
  using get_cb_t = typename easy_t::done_cb_t;
  using string_t = std::string;
 protected:
  ctx_t *ctx_ {nullptr};
  multi_ptr_t multi_ {nullptr};
  size_t nPending_ {0};

  void initTimer() {
    ctx_->getWheel()->addRepeating([this]() {
      update();
    }, std::chrono::milliseconds{50});
  }
  void bindToMulti() {
    multi_->bindCallbacks();
    multi_->setSocketHandler([](CURL *easy, curl_socket_t sockFd, int what, void *pdata) {
      return 0;
    });
    multi_->setTimerHandler([this](long timeoutMsec) {
      ctx_->getWheel()->addOneShot([this]() {
        update();
      }, std::chrono::milliseconds(timeoutMsec));
      return 0;
    });
  }
  EventCurler(ctx_t *ctx, multi_ptr_t&& multi)
    : ctx_(ctx), multi_(std::move(multi)) {}
  void update() {
    auto nPending = perform();
    if (nPending < nPending_) {
      int nRemaining = 0;
      CURLMsg *msg {nullptr};
      do {
        msg = curl_multi_info_read(multi_->get(), &nRemaining);
        if (!!msg && msg->msg == CURLMSG_DONE) {
          auto easyHandle = msg->easy_handle;
          auto original = Easy::fromOwnedEasyHandle(easyHandle);
          original->onFinished();
          delete original;
        }
      } while (!!msg && nRemaining > 0);
    }
    nPending_ = nPending;
  }
 public:
  static EventCurler* createNew(ctx_t *ctx) {
    auto curler = new EventCurler(ctx, multi_ptr_t {multi_t::createNew()});
    curler->initTimer();
    curler->bindToMulti();
    return curler;
  }
  int perform() {
    return multi_->perform();
  }

 protected:
  void addHandle(Easy *handle) {
    handle->bindCallbacks();
    multi_->addHandle(handle);
  }
 public:

  void getURL(const string_t& url, get_cb_t cb) {
    auto easy = Easy::createNew();
    easy->setURL(url);
    easy->setDoneCallback(cb);
    addHandle(easy);
    perform();
  }

  template<typename TCallable,
    typename = typename std::enable_if<!std::is_same<get_cb_t, TCallable>::value, TCallable>::type>
  void getURL(const string_t& url, TCallable &&callable) {
    get_cb_t cb {std::forward<TCallable>(callable)};
    getURL(url, cb);
  }

  template<typename TCallable,
    typename = typename std::enable_if<!std::is_same<get_cb_t, TCallable>::value, TCallable>::type>
  void getURL(const string_t& url, const TCallable &callable) {
    get_cb_t cb {callable};
    getURL(url, cb);
  }

  multi_t* getMulti() {
    return multi_.get();
  }
};

}} // score::curl
