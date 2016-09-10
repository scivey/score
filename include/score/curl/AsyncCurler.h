#pragma once
#include "score/curl/CurlResponse.h"
#include "score/curl/CurlMultiHandle.h"
#include "score/curl/CurlEasyHandle.h"
#include "score/curl/AsyncCurlerContext.h"
#include "score/curl/AsyncCurlerContext-inl.h"
#include "score/curl/AsyncRequestContext.h"

#include <curl/curl.h>
#include <event2/event.h>
#include <string>
#include <sstream>
#include <folly/futures/Future.h>
#include <folly/futures/Promise.h>
#include <folly/io/async/EventBase.h>
#include <memory>

namespace score { namespace curl {

template<typename TCurlApi>
class AsyncCurler {
 public:
  using response_t = CurlResponse;
  using context_t = AsyncCurlerContext<TCurlApi>;
  using request_context_t = AsyncRequestContext<TCurlApi>;
  using request_event_handler = typename AsyncRequestContext<TCurlApi>::EventHandler;

 protected:
  folly::EventBase *base_ {nullptr};
  std::unique_ptr<context_t> ctx_ {nullptr};
  AsyncCurler(folly::EventBase*, std::unique_ptr<context_t>);
  void getInThread(std::string url, request_event_handler*);
 public:
  static AsyncCurler* createPtr(folly::EventBase*);
  void getUrl(std::string url, request_event_handler*);
};


template<typename TCurlApi>
AsyncCurler<TCurlApi>::AsyncCurler(folly::EventBase *base,
    std::unique_ptr<AsyncCurlerContext<TCurlApi>> ctx)
  : base_(base), ctx_(std::forward<decltype(ctx)>(ctx)) {}


template<typename TCurlApi>
AsyncCurler<TCurlApi>* AsyncCurler<TCurlApi>::createPtr(folly::EventBase *base) {
   std::unique_ptr<AsyncCurlerContext<TCurlApi>> ctx {
    AsyncCurlerContext<TCurlApi>::createNew(base->getLibeventBase())
  };
  return new AsyncCurler {base, std::move(ctx)};
}

template<typename TCurlApi>
void AsyncCurler<TCurlApi>::getInThread(std::string url,
    request_event_handler* handler) {
  SDCHECK(!!ctx_);
  auto reqCtx = request_context_t::createNew(
    ctx_->getNewId(),
    handler,
    ctx_.get(),
    url
  );
  auto rc = TCurlApi::multi_add_handle(
    ctx_->getCurlHandle(),
    reqCtx->getCurlHandle()
  );
  CHECK(rc == CURLM_OK);
}

template<typename TCurlApi>
void AsyncCurler<TCurlApi>::getUrl(
    std::string url, request_event_handler *handler) {
  base_->runInEventBaseThread([this, handler, url]() {
    getInThread(url, handler);
  });
}

}} // score::curl
