#pragma once
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include "score/Try.h"
#include "score/Unit.h"
#include "score/Optional.h"
#include "score/io/SocketAddr.h"
#include "score/func/Function.h"
#include "score_async/EventContext.h"
#include "score_async/futures/st/STPromise.h"
#include "score_async/futures/st/STFuture.h"
#include "score_redis/RedisDynamicResponse.h"

namespace score { namespace redis {


class RedisClient: public std::enable_shared_from_this<RedisClient> {
 public:
  using ll_client_t = LLRedisClient;
  using connect_future_t = typename ll_client_t::connect_future_t;
  using disconnect_future_t = typename ll_client_t::connect_future_t;
  using string_t = typename ll_client_t::string_t;
  using redis_signed_t = typename ll_client_t::redis_signed_t;
  using redis_float_t = typename ll_client_t::redis_float_t
  using event_ctx_t = score::async::EventContext;
  using ll_client_ptr_t = std::shared_ptr<ll_client_t>;
 protected:
  event_ctx_t *eventContext_ {nullptr};
  ll_client_ptr_t llClient_ {nullptr};

 public:

  RedisClient(event_ctx_t* ctx, ll_client_ptr_t llClient)
    : eventContext_(ctx), llClient_(llClient){}

  RedisClient(RedisClient&&);
  RedisClient& operator=(RedisClient&&other);
  static RedisClient* createNew(event_ctx_t *ctx, io::SocketAddr&& sockAddr) {
    return new RedisClient {
        ctx,
        util::createShared<LLRedisClient>(
          ctx, std::forward<io::SocketAddr>(sockAddr)
        )
    };
  }

  connect_future_t connect();
  disconnect_future_t disconnect();



};


}} // score::redis


