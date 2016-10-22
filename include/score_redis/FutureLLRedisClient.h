#pragma once
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include "score_redis/LLRedisClient.h"
#include "score_async/futures/st/STPromise.h"
#include "score_async/futures/st/STFuture.h"
struct redisAsyncContext;

namespace score { namespace redis {

class FutureLLRedisClient: public std::enable_shared_from_this<FutureLLRedisClient> {
 public:
  using res_t = RedisDynamicResponse;
 protected:
  template<typename T>
  struct outcome_traits{
    using type = T;
    using as_promise = score::async::futures::st::STPromise<T>;
    using as_future = decltype(std::declval<as_promise>().getFuture());
  };

  using connect_result_traits = outcome_traits<
    score::Try<score::Unit>
  >;
  using disconnect_result_traits = outcome_traits<score::Try<score::Unit>>;
  using res_traits = outcome_traits<res_t>;
 public:
  using connect_promise_t = typename connect_result_traits::as_promise;
  using connect_future_t = typename connect_result_traits::as_future;
  using disconnect_promise_t = typename disconnect_result_traits::as_promise;
  using disconnect_future_t = typename disconnect_result_traits::as_future;
  using res_promise_t = typename res_traits::as_promise;
  using res_future_t = typename res_traits::as_future;
  using arg_str_ref = typename LLRedisClient::arg_str_ref;
  using redis_signed_t = typename LLRedisClient::redis_signed_t;
  using ll_client_ptr_t = std::shared_ptr<LLRedisClient>;
 protected:

  ll_client_ptr_t llClient_ {nullptr};
  disconnect_promise_t disconnectPromise_;
  connect_promise_t connectPromise_;
 public:
  FutureLLRedisClient(ll_client_ptr_t);

  connect_future_t connect();
  disconnect_future_t disconnect();

  res_future_t get(arg_str_ref);
  res_future_t set(arg_str_ref, arg_str_ref);
  res_future_t set(arg_str_ref, redis_signed_t);

  template<typename T = typename LLRedisClient::mget_init_list>
  res_future_t mget(T&& arg) {
    auto promise = mem::make_st_shared<res_promise_t>();
    llClient_->mget(std::forward<T>(arg), [promise](res_t response) {
      promise->setValue(response);
    });
    return promise->getFuture();
  }

  template<typename T = typename LLRedisClient::mset_init_list>
  res_future_t mset(T&& arg) {
    auto promise = mem::make_st_shared<res_promise_t>();
    llClient_->mset(std::forward<T>(arg), [promise](res_t response) {
      promise->setValue(response);
    });
    return promise->getFuture();
  }
};



}} // score::redis


