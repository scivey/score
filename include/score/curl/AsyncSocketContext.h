#pragma once
#include <string>
#include <map>
#include <memory>
#include <folly/futures/Promise.h>
#include <folly/futures/Future.h>
#include <event2/event.h>
#include "score/curl/CurlMultiHandle.h"
#include "score/curl/CurlEasyHandle.h"
#include "score/curl/AsyncCurlerContext.h"
#include "score/curl/CurlResponse.h"

namespace score { namespace curl {

template<typename TCurlApi>
class AsyncSocketContext {
 public:
  using curl_api_policy = TCurlApi;
  using easy_handle_t = CurlEasyHandle<TCurlApi>;
  using parent_t = AsyncCurlerContext<TCurlApi>;
 protected:
  parent_t *parent_ {nullptr};
  curl_socket_t sockFd_ {0};
  easy_handle_t easyHandle_;
  int action_ {0};
  long timeout_ {0};
  struct event *ev_ {nullptr};
  int evSet_ {0};
  AsyncSocketContext(easy_handle_t&&, parent_t*, curl_socket_t, int);
 public:
  static AsyncSocketContext* createNew(
    curl_socket_t sockFd,
    easy_handle_t&&, int action,
    parent_t *parent
  );
  void update(curl_socket_t sockFd, CURL*, int action, parent_t *parent);
  ~AsyncSocketContext();
};


template<typename TCurlApi>
void AsyncSocketContext<TCurlApi>::update(curl_socket_t sockFd,
    CURL *easyPtr, int action,
    AsyncCurlerContext<TCurlApi> *parent) {
  LOG(INFO) << "here : " << sockFd_ << "\t" << sockFd;
  sockFd_ = sockFd;
  // easyHandle_.release();
  easyHandle_ = easyPtr;
  action_ = action;
  LOG(INFO) << "here";
  parent_ = parent;
  if (evSet_) {
    event_free(ev_);
  }
  evSet_ = 0;
  int kind =
    (action & CURL_POLL_IN ? EV_READ : 0) |
    (action & CURL_POLL_OUT ? EV_WRITE: 0) |
    EV_PERSIST;
  LOG(INFO) << "here";
  ev_ = event_new(
    parent->getLibeventBase(),
    sockFd_,
    kind,
    parent_t::libeventEventCallback,
    (void*) parent
  );
  LOG(INFO) << "here";
  evSet_ = 1;
  event_add(ev_, nullptr);
}

template<typename TCurlApi>
AsyncSocketContext<TCurlApi>::AsyncSocketContext(
  CurlEasyHandle<TCurlApi> &&easy, AsyncCurlerContext<TCurlApi> *parent,
  curl_socket_t sock, int action)
  : easyHandle_(std::forward<CurlEasyHandle<TCurlApi>>(easy)),
    parent_(parent),
    sockFd_(sock),
    action_(action) {}


template<typename TCurlApi>
AsyncSocketContext<TCurlApi>* AsyncSocketContext<TCurlApi>::createNew(
    curl_socket_t sockFd, CurlEasyHandle<TCurlApi> &&easy,
    int action, AsyncCurlerContext<TCurlApi> *parent) {
  return new AsyncSocketContext(
    std::forward<CurlEasyHandle<TCurlApi>>(easy),
    parent, sockFd, action
  );
}

template<typename TCurlApi>
AsyncSocketContext<TCurlApi>::~AsyncSocketContext() {
  if (evSet_) {
    event_free(ev_);
  }
}

}} // score::curl