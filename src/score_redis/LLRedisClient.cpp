#include "score_redis/LLRedisClient.h"
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <glog/logging.h>
#include <folly/ExceptionWrapper.h>
#include "score_redis/RedisError.h"
#include "score_redis/hiredis_adapter/hiredis_adapter.h"

using namespace std;

namespace score { namespace redis {

using arg_str_ref = typename LLRedisClient::arg_str_ref;
using cmd_str_ref = typename LLRedisClient::cmd_str_ref;
using redis_signed_t = typename LLRedisClient::redis_signed_t;
using arg_str_list = typename LLRedisClient::arg_str_list;
using mset_init_list = typename LLRedisClient::mset_init_list;
using string_t = typename LLRedisClient::string_t;
using cb_t = typename LLRedisClient::cb_t;
using connect_cb_t = typename LLRedisClient::connect_cb_t;
using disconnect_cb_t = typename LLRedisClient::disconnect_cb_t;
using event_ctx_t = typename LLRedisClient::event_ctx_t;

LLRedisClient::LLRedisClient(event_ctx_t *ctx, const string_t &host, int port)
  : eventContext_(ctx), host_(host), port_(port) {}


LLRedisClient::LLRedisClient(LLRedisClient &&other)
  : eventContext_(other.eventContext_),
    host_(other.host_),
    port_(other.port_),
    redisContext_(other.redisContext_) {
  other.redisContext_ = nullptr;
}

LLRedisClient& LLRedisClient::operator=(LLRedisClient &&other) {
  std::swap(eventContext_, other.eventContext_);
  std::swap(host_, other.host_);
  std::swap(port_, other.port_);
  std::swap(redisContext_, other.redisContext_);
  return *this;
}

std::shared_ptr<LLRedisClient> LLRedisClient::createShared(event_ctx_t *ctx,
      const string_t& host, int port) {
  return std::make_shared<LLRedisClient>(LLRedisClient(ctx, host, port));
}

void LLRedisClient::connect(connect_cb_t&& cb) {
  CHECK(!redisContext_);
  redisContext_ = redisAsyncConnect(host_.c_str(), port_);
  if (redisContext_->err) {
    cb(util::makeTryFailure<shared_ptr<LLRedisClient>, RedisIOError>(
      redisContext_->errstr
    ));
    return;
  }
  connectCallback_.assign(std::forward<connect_cb_t>(cb));
  hiredis_adapter::scoreLibeventAttach(
    this, redisContext_, eventContext_->getBase()->getBase()
  );
  redisAsyncSetConnectCallback(redisContext_,
    &LLRedisClient::hiredisConnectCallback);
  redisAsyncSetDisconnectCallback(redisContext_,
    &LLRedisClient::hiredisDisconnectCallback);
}

void LLRedisClient::disconnect(disconnect_cb_t&& cb) {
  CHECK(!!redisContext_);
  disconnectCallback_.assign(std::forward<disconnect_cb_t>(cb));
  redisAsyncDisconnect(redisContext_);
}

void LLRedisClient::command0(cmd_str_ref cmd, cb_t&& cb) {
  auto reqCtx = new LLRequestContext {
    std::forward<cb_t>(cb)
  };
  redisAsyncCommand(redisContext_,
    &LLRedisClient::hiredisCommandCallback,
    (void*) reqCtx,
    cmd.c_str()
  );
}

void LLRedisClient::command1(cmd_str_ref cmd,
    arg_str_ref arg, cb_t&& cb) {
  auto reqCtx = new LLRequestContext {std::forward<cb_t>(cb)};
  redisAsyncCommand(redisContext_,
    &LLRedisClient::hiredisCommandCallback,
    (void*) reqCtx,
    cmd.c_str(), arg.c_str()
  );
}

void LLRedisClient::command2(cmd_str_ref cmd,
    arg_str_ref arg1, arg_str_ref arg2, cb_t&& cb) {
  auto reqCtx = new LLRequestContext {std::forward<cb_t>(cb)};
  redisAsyncCommand(redisContext_,
    &LLRedisClient::hiredisCommandCallback,
    (void*) reqCtx,
    cmd.c_str(), arg1.c_str(), arg2.c_str()
  );
}

void LLRedisClient::command2(cmd_str_ref cmd,
    arg_str_ref arg1, redis_signed_t arg2, cb_t&& cb) {
  // auto reqCtx = new LLRequestContext {shared_from_this()};
  auto reqCtx = new LLRequestContext {std::forward<cb_t>(cb)};
  redisAsyncCommand(redisContext_,
    &LLRedisClient::hiredisCommandCallback,
    (void*) reqCtx,
    cmd.c_str(), arg1.c_str(), arg2
  );
}


void LLRedisClient::get(arg_str_ref key, cb_t&& cb) {
  command1("GET %s", key, std::forward<cb_t>(cb));
}

// void LLRedisClient::del(arg_str_ref key, cb_t&& cb) {
//   return command1("DEL %s", key, std::forward<cb_t>(cb));
// }

// void LLRedisClient::exists(arg_str_ref key, cb_t&& cb) {
//   return command1("EXISTS %s", key, std::forward<cb_t>(cb));
// }

// LLRedisClient::response_future_t LLRedisClient::expire(arg_str_ref key,
//     redis_signed_t ttlSecs) {
//   return command2("EXPIRE %s %i", key, ttlSecs);
// }

void LLRedisClient::set(arg_str_ref key, arg_str_ref val, cb_t&& cb) {
  command2("SET %s %s", key, val, std::forward<cb_t>(cb));
}

void LLRedisClient::set(arg_str_ref key, redis_signed_t val, cb_t&& cb) {
  command2("SET %s %i", key, val, std::forward<cb_t>(cb));
}

// LLRedisClient::response_future_t LLRedisClient::mset(mset_init_list&& msetList) {
//   folly::fbvector<std::pair<arg_str_t, arg_str_t>> toMset{
//     std::forward<mset_init_list>(msetList)
//   };
//   return mset(toMset);
// }

// LLRedisClient::response_future_t LLRedisClient::mget(mget_init_list&& mgetList) {
//   folly::fbvector<arg_str_t> toMget{
//     std::forward<mget_init_list>(mgetList)
//   };
//   return mget(toMget);
// }

// LLRedisClient::response_future_t LLRedisClient::setnx(arg_str_ref key,
//     arg_str_ref val) {
//   return command2("SETNX %s %s", key, val);
// }

// LLRedisClient::response_future_t LLRedisClient::setnx(arg_str_ref key,
//     redis_signed_t val) {
//   return command2("SETNX %s %i", key, val);
// }

// LLRedisClient::response_future_t LLRedisClient::getset(arg_str_ref key,
//     arg_str_ref val) {
//   return command2("GETSET %s %s", key, val);
// }

// LLRedisClient::response_future_t LLRedisClient::incr(arg_str_ref key) {
//   return command1("INCR %s", key);
// }

// LLRedisClient::response_future_t LLRedisClient::incrby(arg_str_ref key,
//     redis_signed_t amount) {
//   return command2("INCRBY %s %i", key, amount);
// }

// LLRedisClient::response_future_t LLRedisClient::decr(arg_str_ref key) {
//   return command1("DECR %s", key);
// }

// LLRedisClient::response_future_t LLRedisClient::decrby(arg_str_ref key,
//     redis_signed_t amount) {
//   return command2("DECRBY %s %i", key, amount);
// }

// LLRedisClient::response_future_t LLRedisClient::llen(arg_str_ref key) {
//   return command1("LLEN %s", key);
// }

// LLRedisClient::response_future_t LLRedisClient::strlen(arg_str_ref key) {
//   return command1("STRLEN %s", key);
// }

// using subscription_try_t = LLRedisClient::subscription_try_t;
// using subscription_handler_ptr_t = LLRedisClient::subscription_handler_ptr_t;

// subscription_try_t LLRedisClient::subscribe(subscription_handler_ptr_t handler,
//     arg_str_ref channel) {
//   void *userData = nullptr;
//   auto subscription = RedisSubscription::createShared(
//     shared_from_this(),
//     std::forward<subscription_handler_ptr_t>(handler)
//   );
//   currentSubscription_ = subscription;
//   redisAsyncCommand(
//     redisContext_,
//     &LLRedisClient::hiredisSubscriptionCallback,
//     userData,
//     "SUBSCRIBE %s", channel.c_str()
//   );
//   return subscription_try_t { subscription };
// }

void LLRedisClient::hiredisConnectCallback(const redisAsyncContext *ac, int status) {
  auto clientPtr = detail::getClientFromContext(ac);
  clientPtr->handleConnected(status);
}

void LLRedisClient::hiredisDisconnectCallback(const redisAsyncContext *ac, int status) {
  auto clientPtr = detail::getClientFromContext(ac);
  clientPtr->handleDisconnected(status);
}

void LLRedisClient::hiredisCommandCallback(redisAsyncContext *ac, void *reply, void *pdata) {
  auto clientPtr = detail::getClientFromContext(ac);
  auto reqCtx = (LLRequestContext*) pdata;
  auto bareReply = (redisReply*) reply;
  clientPtr->handleCommandResponse(reqCtx, RedisDynamicResponse {bareReply});
}

// void LLRedisClient::hiredisSubscriptionCallback(redisAsyncContext *ac, void *reply, void*) {
//   auto clientPtr = detail::getClientFromContext(ac);
//   auto bareReply = (redisReply*) reply;
//   clientPtr->handleSubscriptionEvent(RedisDynamicResponse {bareReply});
// }

void LLRedisClient::handleConnected(int status) {
  CHECK(status == REDIS_OK);
  auto selfPtr = this->shared_from_this();
  connectCallback_.value()(score::Try<decltype(selfPtr)> {selfPtr});
}

void LLRedisClient::handleCommandResponse(LLRequestContext *ctx, RedisDynamicResponse &&response) {
  ctx->callback(std::forward<RedisDynamicResponse>(response));
}

void LLRedisClient::handleDisconnected(int status) {
  CHECK(status == REDIS_OK);
  if (disconnectCallback_) {
    disconnectCallback_.value()(score::Try<score::Unit> {score::Unit{}});
  }
  // disconnectPromise_.setValue(folly::Try<folly::Unit> {folly::Unit {}});
}

// void LLRedisClient::handleSubscriptionEvent(RedisDynamicResponse&& response) {
//   auto subscriptionPtr = currentSubscription_.lock();
//   if (subscriptionPtr) {
//     subscriptionPtr->dispatchMessage(std::move(response));
//   }
// }

LLRedisClient::~LLRedisClient() {
  if (redisContext_) {
    delete redisContext_;
    redisContext_ = nullptr;
  }
}

namespace detail {
LLRedisClient* getClientFromContext(const redisAsyncContext *ctx) {
  auto evData = (hiredis_adapter::scoreLibeventEvents*) ctx->ev.data;
  return evData->client;
}
}

}} // score::redis

