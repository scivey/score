#pragma once
#include <string>
#include <sstream>
#include <memory>

#include <curl/curl.h>
#include <event2/event.h>
#include <folly/io/async/EventBase.h>

#include "score/curl/detail/CurlMultiHandle.h"
#include "score/curl/detail/CurlEasyHandle.h"
#include "score/curl/detail/AsyncCurlerContext.h"
#include "score/curl/detail/AsyncCurlerContext-inl.h"
#include "score/curl/detail/AsyncRequestContext.h"


namespace score { namespace curl { namespace detail {

template<typename TCurlApi>
class AsyncCurlerBase {
 public:
  using context_t = AsyncCurlerContext<TCurlApi>;
  using request_context_t = AsyncRequestContext<TCurlApi>;
  using request_event_handler = typename AsyncRequestContext<TCurlApi>::EventHandler;

 protected:
  folly::EventBase *base_ {nullptr};
  std::unique_ptr<context_t> ctx_ {nullptr};
  AsyncCurlerBase(folly::EventBase*, std::unique_ptr<context_t>);
  void getInThread(std::string url, request_event_handler*);
 public:
  static AsyncCurlerBase* createPtr(folly::EventBase*);
  void getUrl(std::string url, request_event_handler*);
};


template<typename TCurlApi>
AsyncCurlerBase<TCurlApi>::AsyncCurlerBase(folly::EventBase *base,
    std::unique_ptr<AsyncCurlerContext<TCurlApi>> ctx)
  : base_(base), ctx_(std::forward<decltype(ctx)>(ctx)) {}


template<typename TCurlApi>
AsyncCurlerBase<TCurlApi>* AsyncCurlerBase<TCurlApi>::createPtr(folly::EventBase *base) {
   std::unique_ptr<AsyncCurlerContext<TCurlApi>> ctx {
    AsyncCurlerContext<TCurlApi>::createNew(base->getLibeventBase())
  };
  return new AsyncCurlerBase {base, std::move(ctx)};
}

template<typename TCurlApi>
void AsyncCurlerBase<TCurlApi>::getInThread(std::string url,
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
void AsyncCurlerBase<TCurlApi>::getUrl(
    std::string url, request_event_handler *handler) {
  base_->runInEventBaseThread([this, handler, url]() {
    getInThread(url, handler);
  });
}

}}} // score::curl::detail
