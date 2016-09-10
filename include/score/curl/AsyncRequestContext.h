#pragma once
#include <string>
#include <map>
#include <memory>
#include <event2/event.h>
#include <stdexcept>
#include <exception>
#include "score/curl/CurlMultiHandle.h"
#include "score/curl/CurlResponse.h"
#include "score/curl/AsyncCurlerContext.h"

namespace score { namespace curl {

template<typename TCurlApi>
class AsyncRequestContext {
 public:
  using curl_api_policy = TCurlApi;
  using easy_handle_t = CurlEasyHandle<curl_api_policy>;
  using parent_t = AsyncCurlerContext<TCurlApi>;

  class EventHandler {
   public:
    virtual void onFinished() = 0;
    virtual size_t onReadBody(char *data, size_t size, size_t nMembers) = 0;
    virtual size_t onReadHeader(char *data, size_t size, size_t nMembers) = 0;
    virtual void onError(const std::exception &err) = 0;
    virtual char* getErrorBuffer() = 0;
  };

 protected:
  size_t id_ {0};
  easy_handle_t easyHandle_;
  EventHandler *handler_ {nullptr};
  parent_t *parent_ {nullptr};
  std::string url_;
  AsyncRequestContext(size_t id, easy_handle_t &&easy,
    EventHandler *handler, parent_t *parent, std::string url)
    : id_(id),
      easyHandle_(std::forward<easy_handle_t>(easy)),
      handler_(handler),
      parent_(parent),
      url_(url) {}

  void setCurlOptions() {
    easyHandle_.setOpt(CURLOPT_WRITEFUNCTION, AsyncRequestContext::curlWriteCallback);
    easyHandle_.setOpt(CURLOPT_WRITEDATA, (void*) this);
    easyHandle_.setOpt(CURLOPT_HEADERFUNCTION, AsyncRequestContext::curlHeaderCallback);
    easyHandle_.setOpt(CURLOPT_HEADERDATA, (void*) this);
    easyHandle_.setOpt(CURLOPT_URL, url_.c_str());
    easyHandle_.setOpt(CURLOPT_VERBOSE, 1L);
    easyHandle_.setOpt(CURLOPT_ERRORBUFFER, handler_->getErrorBuffer());
    easyHandle_.setOpt(CURLOPT_PRIVATE, this);
    easyHandle_.setOpt(CURLOPT_NOPROGRESS, 0L);
    // easyHandle_.setOpt(CURLOPT_PROGRESSFUNCTION, AsyncRequestContext::curlProgressCallback);
    // easyHandle_.setOpt(CURLOPT_PROGRESSDATA, this);
  }
 public:
  size_t getId() const {
    return id_;
  }

  static size_t curlWriteCallback(void *data, size_t size,
      size_t nMembers, void *pdata) {
    auto ctx = (AsyncRequestContext*) pdata;
    return ctx->handler_->onReadBody((char*) data, size, nMembers);
  }

  static size_t curlHeaderCallback(void *data, size_t size,
      size_t nMembers, void *pdata) {
    auto ctx = (AsyncRequestContext*) pdata;
    return ctx->handler_->onReadHeader((char*) data, size, nMembers);
  }

  static size_t curlProgressCallback(void *pdata, double dlTotal,
      double dlNow, double ult, double uln) {
    // auto ctx = (AsyncRequestContext*) pdata;
    // return ctx->onCurlProgress(dlTotal, dlNow, ult, uln);
    return 0;
  }

  size_t onCurlProgress(double dlTotal, double dlNow, double ult, double uln) {
    return 0;
  }

  CURL* getCurlHandle() {
    return easyHandle_.getCurlHandle();
  }

  static AsyncRequestContext* createNew(size_t id,
      EventHandler *handler, parent_t *parent, std::string url) {
    auto instance = new AsyncRequestContext(
      id, easy_handle_t::create(), handler, parent, url
    );
    instance->setCurlOptions();
    return instance;
  }

  void triggerFinished() {
    handler_->onFinished();
  }

};

}} // score::curl
