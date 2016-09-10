#pragma once

#include <curl/curl.h>
#include <folly/Optional.h>
#include <folly/ExceptionWrapper.h>
#include "score/curl/detail/AsyncCurlerBase.h"
#include "score/curl/detail/AsyncRequestContext.h"
#include "score/curl/detail/RealCurlAPI.h"
#include "score/curl/RawCurlResponse.h"
#include "score/curl/CurlError.h"


namespace score { namespace curl {

template<typename TCurlAPI>
class SimpleCurlerBase {
 public:
  using curl_api_policy = TCurlAPI;
  using base_t = detail::AsyncCurlerBase<TCurlAPI>;
  using request_ctx_t = detail::AsyncRequestContext<TCurlAPI>;
  using error_option_t = folly::Optional<folly::exception_wrapper>;
  using response_t = RawCurlResponse;
  using response_option_t = folly::Optional<RawCurlResponse>;

  class RequestEventHandler : public request_ctx_t::EventHandler {
   public:
    using func_t = std::function<void (error_option_t, response_option_t)>;
   protected:
    func_t cb_;
    char errorBuffer_[CURL_ERROR_SIZE];
    RawCurlResponse response_;
   public:
    RequestEventHandler(func_t &&func)
      : cb_(std::forward<func_t>(func)) {}

    RequestEventHandler(const func_t &func)
      : cb_(func) {}

    template<typename TCallable>
    RequestEventHandler(TCallable &&func)
      : cb_(std::forward<TCallable>(func)) {}

    template<typename TCallable>
    RequestEventHandler(const TCallable &func)
      : cb_(func) {}

    void onFinished() override {
      error_option_t noError;
      response_option_t aResponse {std::move(response_)};
      cb_(std::move(noError), std::move(aResponse));
    }

    void onError(const CurlError &ex) override {
      error_option_t anError {
        folly::make_exception_wrapper<CurlError>(ex)
      };
      response_option_t noResponse;
      cb_(std::move(anError), std::move(noResponse));
    }

    size_t onReadBody(char *data, size_t size, size_t nMembers) override {
      size_t dataLen = size * nMembers;
      char *current = data;
      while (current != data + dataLen) {
        response_.bodyBuffer << *current;
        ++current;
      }
      return dataLen;
    }
    size_t onReadHeader(char *data, size_t size, size_t nMembers) override {
      size_t dataLen = size * nMembers;
      char *current = data;
      std::ostringstream headerBuff;
      while (current != data + dataLen) {
        headerBuff << *current;
        ++current;
      }
      response_.rawHeaders.push_back(headerBuff.str());
      return dataLen;
    }
    char* getErrorBuffer() override {
      return errorBuffer_;
    }
  };
 protected:
  base_t *base_ {nullptr};
  SimpleCurlerBase(base_t *base): base_(base){}
 public:
  static SimpleCurlerBase* createPtr(folly::EventBase *evBase) {
    return new SimpleCurlerBase(base_t::createPtr(evBase));
  }

  template<typename TCallable>
  void getUrl(std::string url, TCallable &&callback) {
    auto handler = new RequestEventHandler(std::forward<TCallable>(callback));
    base_->getUrl(url, handler);
  }
};

using SimpleCurler = SimpleCurlerBase<detail::RealCurlAPI>;

}} // score::curl
