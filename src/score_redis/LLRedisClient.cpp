#include "score_redis/LLRedisClient.h"
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <glog/logging.h>
#include <folly/ExceptionWrapper.h>
#include "score_redis/RedisError.h"
#include "score_redis/hiredis_adapter/hiredis_adapter.h"
#include "score_redis/hiredis_adapter/LibeventRedisContext.h"
#include "score_async/futures/st/helpers.h"

using namespace std;
namespace futures = score::async::futures;


namespace score { namespace redis {

using arg_str_ref = typename LLRedisClient::arg_str_ref;
using cmd_str_ref = typename LLRedisClient::cmd_str_ref;
using redis_signed_t = typename LLRedisClient::redis_signed_t;
using arg_str_list = typename LLRedisClient::arg_str_list;
using mset_init_list = typename LLRedisClient::mset_init_list;
using string_t = typename LLRedisClient::string_t;
using cb_t = typename LLRedisClient::cb_t;
using event_ctx_t = typename LLRedisClient::event_ctx_t;
using connect_future_t = typename LLRedisClient::connect_future_t;
using disconnect_future_t = typename LLRedisClient::disconnect_future_t;
using score::io::SocketAddr;


LLRedisClient::LLRedisClient(event_ctx_t *ctx, SocketAddr&& addr)
  : eventContext_(ctx), serverAddr_(std::forward<SocketAddr>(addr)) {}


LLRedisClient::LLRedisClient(LLRedisClient &&other)
  : eventContext_(other.eventContext_),
    serverAddr_(other.serverAddr_),
    redisContext_(other.redisContext_),
    adapter_(other.adapter_) {
  other.redisContext_ = nullptr;
}

LLRedisClient& LLRedisClient::operator=(LLRedisClient &&other) {
  std::swap(eventContext_, other.eventContext_);
  std::swap(serverAddr_, other.serverAddr_);
  std::swap(adapter_, other.adapter_);
  std::swap(redisContext_, other.redisContext_);
  return *this;
}

LLRedisClient::RequestContext::RequestContext(cb_t&& cb)
  : callback(std::forward<cb_t>(cb)) {}


LLRedisClient* LLRedisClient::createNew(event_ctx_t *ctx,
    SocketAddr&& addr) {
  return new LLRedisClient{ctx, std::forward<SocketAddr>(addr)};
}

connect_future_t LLRedisClient::connect() {
  if (redisContext_) {
    return futures::st::makeReadySTFuture(
      util::makeTryFailure<Unit, AlreadyConnected>("Client is already connected.")
    );
  }
  DCHECK(!redisContext_);
  redisContext_ = redisAsyncConnect(
    serverAddr_.getHost().c_str(),
    serverAddr_.getPort()
  );

  // hiredis claims to never return null here, but still
  DCHECK(!!redisContext_);

  if (redisContext_->err) {
    return futures::st::makeReadySTFuture(
      util::makeTryFailure<Unit, RedisIOError>(redisContext_->errstr)
    );
  }
  adapter_ = hiredis_adapter::scoreLibeventAttach(
    this, redisContext_, eventContext_->getBase()->getBase()
  );
  redisAsyncSetConnectCallback(redisContext_,
    &LLRedisClient::hiredisConnectCallback);
  redisAsyncSetDisconnectCallback(redisContext_,
    &LLRedisClient::hiredisDisconnectCallback);
  return connectPromise_.getFuture();
}

disconnect_future_t LLRedisClient::disconnect() {
  if (!redisContext_) {
    return futures::st::makeReadySTFuture(util::makeTryFailure<Unit, NotConnected>(
      "Client is not connected."
    ));
  }
  redisAsyncDisconnect(redisContext_);
  return disconnectPromise_.getFuture();
}

void LLRedisClient::command0(cmd_str_ref cmd, cb_t&& cb) {
  auto reqCtx = new RequestContext {
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
  auto reqCtx = new RequestContext {std::forward<cb_t>(cb)};
  redisAsyncCommand(redisContext_,
    &LLRedisClient::hiredisCommandCallback,
    (void*) reqCtx,
    cmd.c_str(), arg.c_str()
  );
}

void LLRedisClient::command2(cmd_str_ref cmd,
    arg_str_ref arg1, arg_str_ref arg2, cb_t&& cb) {
  auto reqCtx = new RequestContext {std::forward<cb_t>(cb)};
  redisAsyncCommand(redisContext_,
    &LLRedisClient::hiredisCommandCallback,
    (void*) reqCtx,
    cmd.c_str(), arg1.c_str(), arg2.c_str()
  );
}

void LLRedisClient::command2(cmd_str_ref cmd,
    arg_str_ref arg1, redis_signed_t arg2, cb_t&& cb) {
  // auto reqCtx = new LLRequestContext {shared_from_this()};
  auto reqCtx = new RequestContext {std::forward<cb_t>(cb)};
  redisAsyncCommand(redisContext_,
    &LLRedisClient::hiredisCommandCallback,
    (void*) reqCtx,
    cmd.c_str(), arg1.c_str(), arg2
  );
}


void LLRedisClient::get(arg_str_ref key, cb_t&& cb) {
  command1("GET %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::publish(arg_str_ref channel, arg_str_ref message, cb_t&& cb) {
  command2("PUBLISH %s %s", channel, message, std::forward<cb_t>(cb));
}

void LLRedisClient::del(arg_str_ref key, cb_t&& cb) {
  return command1("DEL %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::exists(arg_str_ref key, cb_t&& cb) {
  return command1("EXISTS %s", key, std::forward<cb_t>(cb));
}

// void LLRedisClient::expire(arg_str_ref key,
//     redis_signed_t ttlSecs) {
//   return command2("EXPIRE %s %i", key, ttlSecs);
// }

void LLRedisClient::set(arg_str_ref key, arg_str_ref val, cb_t&& cb) {
  command2("SET %s %s", key, val, std::forward<cb_t>(cb));
}

void LLRedisClient::set(arg_str_ref key, redis_signed_t val, cb_t&& cb) {
  command2("SET %s %i", key, val, std::forward<cb_t>(cb));
}

void LLRedisClient::mSet(mset_init_list&& msetList, cb_t&& cb) {
  std::vector<std::pair<arg_str_t, arg_str_t>> toMset{
    std::forward<mset_init_list>(msetList)
  };
  mSet(std::move(toMset), std::forward<cb_t>(cb));
}

void LLRedisClient::mGet(mget_init_list&& mgetList, cb_t&& cb) {
  std::vector<arg_str_t> toMget{
    std::forward<mget_init_list>(mgetList)
  };
  mGet(std::move(toMget), std::forward<cb_t>(cb));
}

void LLRedisClient::setNX(arg_str_ref key,
    arg_str_ref val, cb_t&& cb) {
  command2("SETNX %s %s", key, val, std::forward<cb_t>(cb));
}

void LLRedisClient::setNX(arg_str_ref key,
    redis_signed_t val, cb_t&& cb) {
  command2("SETNX %s %i", key, val, std::forward<cb_t>(cb));
}

void LLRedisClient::getSet(arg_str_ref key,
    arg_str_ref val, cb_t&& cb) {
  command2("GETSET %s %s", key, val, std::forward<cb_t>(cb));
}

void LLRedisClient::incr(arg_str_ref key, cb_t&& cb) {
  command1("INCR %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::incrBy(arg_str_ref key,
    redis_signed_t amount, cb_t&& cb) {
  command2("INCRBY %s %i", key, amount, std::forward<cb_t>(cb));
}

void LLRedisClient::decr(arg_str_ref key, cb_t&& cb) {
  command1("DECR %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::decrBy(arg_str_ref key,
    redis_signed_t amount, cb_t&& cb) {
  command2("DECRBY %s %i", key, amount, std::forward<cb_t>(cb));
}

void LLRedisClient::lLen(arg_str_ref key, cb_t&& cb) {
  command1("LLEN %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::strLen(arg_str_ref key, cb_t&& cb) {
  command1("STRLEN %s", key, std::forward<cb_t>(cb));
}

using subscription_try_t = LLRedisClient::subscription_try_t;
using subscription_handler_ptr_t = LLRedisClient::subscription_handler_ptr_t;

subscription_try_t LLRedisClient::subscribe(arg_str_ref channel,
    subscription_handler_ptr_t handler) {
  void *userData = nullptr;
  auto subscription = LLRedisSubscription::createShared(
    this->shared_from_this(),
    std::forward<subscription_handler_ptr_t>(handler)
  );
  currentSubscription_ = subscription;
  redisAsyncCommand(
    redisContext_,
    &LLRedisClient::hiredisSubscriptionCallback,
    userData,
    "SUBSCRIBE %s", channel.c_str()
  );
  return subscription_try_t { subscription };
}

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
  auto reqCtx = (RequestContext*) pdata;
  auto bareReply = (redisReply*) reply;
  CHECK(!!bareReply);
  clientPtr->handleCommandResponse(reqCtx, RedisDynamicResponse {bareReply});
}

void LLRedisClient::hiredisSubscriptionCallback(redisAsyncContext *ac, void *reply, void*) {
  auto clientPtr = detail::getClientFromContext(ac);
  auto bareReply = (redisReply*) reply;
  clientPtr->handleSubscriptionEvent(RedisDynamicResponse {bareReply});
}

void LLRedisClient::handleConnected(int status) {
  CHECK(status == REDIS_OK);
  connectPromise_.setValue(util::makeTrySuccess<Unit>());
}

void LLRedisClient::handleCommandResponse(RequestContext *ctx, RedisDynamicResponse &&response) {
  ctx->callback(util::makeTrySuccess<RedisDynamicResponse>(std::forward<RedisDynamicResponse>(response)));
  delete ctx;
}

void LLRedisClient::handleDisconnected(int status) {
  CHECK(status == REDIS_OK);
  disconnectPromise_.setValue(util::makeTrySuccess<Unit>());
}

void LLRedisClient::handleSubscriptionEvent(RedisDynamicResponse&& response) {
  auto subscriptionPtr = currentSubscription_.lock();
  if (subscriptionPtr) {
    subscriptionPtr->dispatchMessage(std::move(response));
  }
}

void LLRedisClient::maybeCleanupContext() {
  if (redisContext_) {
    delete redisContext_;
    redisContext_ = nullptr;
  }
}

LLRedisClient::~LLRedisClient() {
  maybeCleanupContext();
}

namespace detail {
LLRedisClient* getClientFromContext(const redisAsyncContext *ctx) {
  auto evData = (hiredis_adapter::LibeventRedisContext::InnerContext*) ctx->ev.data;
  return evData->clientPtr;
}
}

}} // score::redis

