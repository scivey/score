#pragma once

#include <folly/futures/Future.h>
#include <folly/futures/Promise.h>
#include <memory>
#include "fredis/redis/RedisDynamicResponse.h"

namespace fredis { namespace redis {

class LLRedisClient;

class LLRedisRequestContext {
 public:
  using response_t = RedisDynamicResponse;
  using response_promise_t = folly::Promise<response_t>;
  using response_future_t = decltype(
    std::declval<response_promise_t>().getFuture()
  );
 protected:
  std::shared_ptr<LLRedisClient> client_;
  response_promise_t donePromise_;
 public:
  LLRedisRequestContext(std::shared_ptr<LLRedisClient>);
  response_future_t getFuture();

  template<typename T>
  void setValue(T&& result) {
    donePromise_.setValue(std::forward<T>(result));
  }

  template<typename T>
  void setValue(const T& result) {
    donePromise_.setValue(result);
  }

};

}} // fredis::redis
