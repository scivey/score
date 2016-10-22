#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include "score_redis/FutureLLRedisClient.h"
#include "score_async/futures/st/STPromise.h"
#include "score_async/futures/st/STFuture.h"
#include "score/mem/st_shared_ptr.h"

struct redisAsyncContext;

namespace score { namespace redis {

using ll_client_ptr_t = typename FutureLLRedisClient::ll_client_ptr_t;
using arg_str_ref = typename FutureLLRedisClient::arg_str_ref;
using redis_signed_t = typename FutureLLRedisClient::redis_signed_t;
using res_future_t = typename FutureLLRedisClient::res_future_t;
using res_promise_t = typename FutureLLRedisClient::res_promise_t;
using res_t = typename FutureLLRedisClient::res_t;

using connect_future_t = typename FutureLLRedisClient::connect_future_t;
using disconnect_future_t = typename FutureLLRedisClient::disconnect_future_t;

FutureLLRedisClient::FutureLLRedisClient(ll_client_ptr_t llClient)
  : llClient_(llClient){}

res_future_t FutureLLRedisClient::get(arg_str_ref key) {
  auto promise = mem::make_st_shared<res_promise_t>();
  llClient_->get(key, [promise](res_t response) {
    promise->setValue(response);
  });
  return promise->getFuture();
}

res_future_t FutureLLRedisClient::set(arg_str_ref key, arg_str_ref val) {
  auto promise = mem::make_st_shared<res_promise_t>();
  llClient_->set(key, val, [promise](res_t response) {
    promise->setValue(response);
  });
  return promise->getFuture();
}

res_future_t FutureLLRedisClient::set(arg_str_ref key, redis_signed_t val) {
  auto promise = mem::make_st_shared<res_promise_t>();
  llClient_->set(key, val, [promise](res_t response) {
    promise->setValue(response);
  });
  return promise->getFuture();
}

connect_future_t FutureLLRedisClient::connect() {
  auto promise = mem::make_st_shared<connect_promise_t>();
  llClient_->connect([promise](score::Try<std::shared_ptr<LLRedisClient>> result) {
    result.throwIfFailed();
    promise->setValue(score::Try<Unit>{Unit{}});
  });
  return promise->getFuture();
}

disconnect_future_t FutureLLRedisClient::disconnect() {
  auto promise = mem::make_st_shared<disconnect_promise_t>();
  llClient_->disconnect([promise](score::Try<score::Unit> result) {
    promise->setValue(std::move(result));
  });
  return promise->getFuture();
}

}} // score::redis


