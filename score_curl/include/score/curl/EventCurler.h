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

  void initTimer();
  void bindToMulti();
  EventCurler(ctx_t *ctx, multi_ptr_t&& multi);
  void update();
 public:
  static EventCurler* createNew(ctx_t *ctx);
  int perform();

 protected:
  void addHandle(Easy *handle);
 public:

  void getURL(const string_t& url, get_cb_t cb);

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

  multi_t* getMulti();
};

}} // score::curl
