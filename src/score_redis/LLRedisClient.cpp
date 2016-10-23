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
using redis_float_t = typename LLRedisClient::redis_float_t;
using arg_str_list = typename LLRedisClient::arg_str_list;
using mset_init_list = typename LLRedisClient::mset_init_list;
using string_t = typename LLRedisClient::string_t;
using cb_t = typename LLRedisClient::cb_t;
using event_ctx_t = typename LLRedisClient::event_ctx_t;
using connect_future_t = typename LLRedisClient::connect_future_t;
using disconnect_future_t = typename LLRedisClient::disconnect_future_t;
using score::io::SocketAddr;
using RequestContext = LLRedisClient::RequestContext;

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

template<typename T>
void command1Impl(redisAsyncContext *redisCtx, cmd_str_ref cmd,
    T arg, cb_t&& cb) {
  auto reqCtx = new RequestContext {std::forward<cb_t>(cb)};
  redisAsyncCommand(redisCtx,
    &LLRedisClient::hiredisCommandCallback,
    (void*) reqCtx,
    cmd.c_str(), arg
  );
}


void LLRedisClient::command1(cmd_str_ref cmd,
    arg_str_ref arg, cb_t&& cb) {
  command1Impl(redisContext_, cmd, arg.c_str(), std::forward<cb_t>(cb));
}

void LLRedisClient::command1(cmd_str_ref cmd,
    redis_signed_t arg, cb_t&& cb) {
  command1Impl(redisContext_, cmd, arg, std::forward<cb_t>(cb));
}

template<typename T1, typename T2>
void command2Impl(redisAsyncContext *redisCtx, cmd_str_ref cmd,
    T1 arg1, T2 arg2, cb_t&& cb) {
  auto reqCtx = new RequestContext {std::forward<cb_t>(cb)};
  redisAsyncCommand(redisCtx,
    &LLRedisClient::hiredisCommandCallback,
    (void*) reqCtx,
    cmd.c_str(), arg1, arg2
  );
}

void LLRedisClient::command2(cmd_str_ref cmd,
    arg_str_ref arg1, arg_str_ref arg2, cb_t&& cb) {
  command2Impl(redisContext_, cmd,
    arg1.c_str(), arg2.c_str(),
    std::forward<cb_t>(cb)
  );
}

void LLRedisClient::command2(cmd_str_ref cmd,
    arg_str_ref arg1, redis_signed_t arg2, cb_t&& cb) {
  command2Impl(redisContext_, cmd,
    arg1.c_str(), arg2,
    std::forward<cb_t>(cb)
  );
}

void LLRedisClient::command2(cmd_str_ref cmd,
    arg_str_ref arg1, redis_float_t arg2, cb_t&& cb) {
  command2Impl(redisContext_, cmd,
    arg1.c_str(), arg2,
    std::forward<cb_t>(cb)
  );
}



template<typename T1, typename T2, typename T3>
void command3Impl(redisAsyncContext *redisCtx, cmd_str_ref cmd,
    T1 arg1, T2 arg2, T3 arg3, cb_t&& cb) {
  auto reqCtx = new RequestContext {std::forward<cb_t>(cb)};
  redisAsyncCommand(redisCtx,
    &LLRedisClient::hiredisCommandCallback,
    (void*) reqCtx,
    cmd.c_str(), arg1, arg2, arg3
  );
}

void LLRedisClient::command3(cmd_str_ref cmd, arg_str_ref arg1,
    arg_str_ref arg2, arg_str_ref arg3, cb_t&& cb) {
  command3Impl(redisContext_, cmd,
    arg1.c_str(), arg2.c_str(), arg3.c_str(),
    std::forward<cb_t>(cb)
  );
}

void LLRedisClient::command3(cmd_str_ref cmd, arg_str_ref arg1,
    redis_signed_t arg2, arg_str_ref arg3, cb_t&& cb) {
  command3Impl(redisContext_, cmd,
    arg1.c_str(), arg2, arg3.c_str(),
    std::forward<cb_t>(cb)
  );
}

void LLRedisClient::command3(cmd_str_ref cmd, arg_str_ref arg1,
    redis_signed_t arg2, redis_signed_t arg3, cb_t&& cb) {
  command3Impl(redisContext_, cmd,
    arg1.c_str(), arg2, arg3,
    std::forward<cb_t>(cb)
  );
}

void LLRedisClient::command3(cmd_str_ref cmd, arg_str_ref arg1,
    arg_str_ref arg2, redis_signed_t arg3, cb_t&& cb) {
  command3Impl(redisContext_, cmd,
    arg1.c_str(), arg2.c_str(), arg3,
    std::forward<cb_t>(cb)
  );
}





void LLRedisClient::decr(arg_str_ref key, cb_t&& cb) {
  command1("DECR %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::decrBy(arg_str_ref key,
    redis_signed_t amount, cb_t&& cb) {
  command2("DECRBY %s %i", key, amount, std::forward<cb_t>(cb));
}

void LLRedisClient::del(arg_str_ref key, cb_t&& cb) {
  return command1("DEL %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::exec(cb_t&& cb) {
  command0("EXEC", std::forward<cb_t>(cb));
}


void LLRedisClient::echo(arg_str_ref key, cb_t&& cb) {
  return command1("ECHO %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::exists(arg_str_ref key, cb_t&& cb) {
  return command1("EXISTS %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::expire(arg_str_ref key,
    redis_signed_t ttlSecs, cb_t&& cb) {
  return command2("EXPIRE %s %i", key, ttlSecs, std::forward<cb_t>(cb));
}

void LLRedisClient::expireAt(arg_str_ref key,
    redis_signed_t unixTime, cb_t&& cb) {
  return command2("EXPIREAT %s %i", key, unixTime, std::forward<cb_t>(cb));
}

void LLRedisClient::flushAll(cb_t&& cb) {
  return command0("FLUSHALL", std::forward<cb_t>(cb));
}

void LLRedisClient::flushDB(cb_t&& cb) {
  return command0("FLUSHDB", std::forward<cb_t>(cb));
}

void LLRedisClient::get(arg_str_ref key, cb_t&& cb) {
  command1("GET %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::getBit(arg_str_ref key, redis_signed_t offset, cb_t&& cb) {
  command2("GETBIT %s %i", key, offset, std::forward<cb_t>(cb));
}

void LLRedisClient::getRange(arg_str_ref key, redis_signed_t start,
    redis_signed_t stop, cb_t&& cb) {
  command3("GETRANGE %s %i %i", key, start, stop, std::forward<cb_t>(cb));
}

void LLRedisClient::getSet(arg_str_ref key,
    arg_str_ref val, cb_t&& cb) {
  command2("GETSET %s %s", key, val, std::forward<cb_t>(cb));
}

void LLRedisClient::hExists(arg_str_ref key, arg_str_ref field, cb_t&& cb) {
  command2("HEXISTS %s %s", key, field, std::forward<cb_t>(cb));
}

void LLRedisClient::hGet(arg_str_ref key, arg_str_ref field, cb_t&& cb) {
  command2("HGET %s %s", key, field, std::forward<cb_t>(cb));
}

void LLRedisClient::hGetAll(arg_str_ref key, cb_t&& cb) {
  command1("HGETALL %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::hIncrBy(arg_str_ref key, arg_str_ref field,
    redis_signed_t amount, cb_t&& cb) {
  command3("HINCRBY %s %s %i", key, field, amount, std::forward<cb_t>(cb));
}

void LLRedisClient::hIncrByFloat(arg_str_ref key, arg_str_ref field,
    redis_float_t amount, cb_t&& cb) {
  command3("HINCRBYFLOAT %s %s %f", key, field, amount, std::forward<cb_t>(cb));
}

void LLRedisClient::hKeys(arg_str_ref key, cb_t&& cb) {
  command1("HKEYS %s", key, std::forward<cb_t>(cb));
}

// HMGET, HMSET

void LLRedisClient::hSet(arg_str_ref key, arg_str_ref field,
    arg_str_ref val, cb_t&& cb) {
  command3("HSET %s %s %s", key, field, val, std::forward<cb_t>(cb));
}

void LLRedisClient::hSetNX(arg_str_ref key, arg_str_ref field,
    arg_str_ref val, cb_t&& cb) {
  command3("HSETNX %s %s %s", key, field, val, std::forward<cb_t>(cb));
}

void LLRedisClient::hStrLen(arg_str_ref key, arg_str_ref field, cb_t&& cb) {
  command2("HSTRLEN %s %s %s", key, field, std::forward<cb_t>(cb));
}

void LLRedisClient::hVals(arg_str_ref key, cb_t&& cb) {
  command1("HVALS %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::incr(arg_str_ref key, cb_t&& cb) {
  command1("INCR %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::incrBy(arg_str_ref key,
    redis_signed_t amount, cb_t&& cb) {
  command2("INCRBY %s %i", key, amount, std::forward<cb_t>(cb));
}

void LLRedisClient::incrByFloat(arg_str_ref key,
    redis_float_t amount, cb_t&& cb) {
  command2("INCRBYFLOAT %s %f", key, amount, std::forward<cb_t>(cb));
}


void LLRedisClient::lIndex (arg_str_ref key,
    redis_signed_t idx, cb_t&& cb) {
  command2("LINDEX %s %i", key, idx, std::forward<cb_t>(cb));
}

void LLRedisClient::lLen(arg_str_ref key, cb_t&& cb) {
  command1("LLEN %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::lPop(arg_str_ref key, cb_t&& cb) {
  command1("LPOP %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::lPushX(arg_str_ref key, arg_str_ref val, cb_t&& cb) {
  command2("LPUSHX %s %s", key, val, std::forward<cb_t>(cb));
}

void LLRedisClient::lRange(arg_str_ref key, redis_signed_t start,
    redis_signed_t stop, cb_t&& cb) {
  command3("LRANGE %s %i %i", key, start, stop, std::forward<cb_t>(cb));
}

void LLRedisClient::lRem(arg_str_ref key, redis_signed_t count,
    arg_str_ref val, cb_t&& cb) {
  command3("LREM %s %i %s", key, count, val, std::forward<cb_t>(cb));
}

void LLRedisClient::lSet(arg_str_ref key, redis_signed_t idx,
    arg_str_ref val, cb_t&& cb) {
  command3("LSET %s %i %s", key, idx, val, std::forward<cb_t>(cb));
}

void LLRedisClient::lTrim(arg_str_ref key, redis_signed_t start,
    redis_signed_t stop, cb_t&& cb) {
  command3("LTRIM %s %i %i", key, start, stop, std::forward<cb_t>(cb));
}

void LLRedisClient::mGet(mget_init_list&& mgetList, cb_t&& cb) {
  std::vector<arg_str_t> toMget{
    std::forward<mget_init_list>(mgetList)
  };
  mGet(std::move(toMget), std::forward<cb_t>(cb));
}

void LLRedisClient::mSet(mset_init_list&& msetList, cb_t&& cb) {
  std::vector<std::pair<arg_str_t, arg_str_t>> toMset{
    std::forward<mset_init_list>(msetList)
  };
  mSet(std::move(toMset), std::forward<cb_t>(cb));
}

void LLRedisClient::multi(cb_t&& cb) {
  command0("MULTI", std::forward<cb_t>(cb));
}

void LLRedisClient::persist(arg_str_ref key, cb_t&& cb) {
  command1("PERSIST %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::pExpire(arg_str_ref key, redis_signed_t ttl, cb_t&& cb) {
  command2("PEXPIRE %s %i", key, ttl, std::forward<cb_t>(cb));
}

void LLRedisClient::pExpireAt(arg_str_ref key, redis_signed_t killAt, cb_t&& cb) {
  command2("PEXPIREAT %s %i", key, killAt, std::forward<cb_t>(cb));
}

// PFADD, PFCOUNT, PFMERGE

void LLRedisClient::ping(arg_str_ref msg, cb_t&& cb) {
  command1("PING %s", msg, std::forward<cb_t>(cb));
}

void LLRedisClient::pSetEx(arg_str_ref key, redis_signed_t msecTTL, cb_t&& cb) {
  command2("PSETEX %s %i", key, msecTTL, std::forward<cb_t>(cb));
}

// PSUBSCRIBE, PUBSUB

void LLRedisClient::pTTL(arg_str_ref key, cb_t&& cb) {
  command1("PTTL %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::publish(arg_str_ref channel, arg_str_ref message, cb_t&& cb) {
  command2("PUBLISH %s %s", channel, message, std::forward<cb_t>(cb));
}

void LLRedisClient::randomKey(cb_t&& cb) {
  command0("RANDOMKEY", std::forward<cb_t>(cb));
}

void LLRedisClient::rename(arg_str_ref oldKey, arg_str_ref newKey, cb_t&& cb) {
  command2("RENAME %s %s", oldKey, newKey, std::forward<cb_t>(cb));
}

void LLRedisClient::renameNX(arg_str_ref oldKey, arg_str_ref newKey, cb_t&& cb) {
  command2("RENAMENX %s %s", oldKey, newKey, std::forward<cb_t>(cb));
}

// RESTORE, ROLE

void LLRedisClient::rPop(arg_str_ref key, cb_t&& cb) {
  command1("RPOP %s", key, std::forward<cb_t>(cb));
}

void LLRedisClient::rPopLPush(arg_str_ref src, arg_str_ref dest, cb_t&& cb) {
  command2("RPOPLPUSH %s %s", src, dest, std::forward<cb_t>(cb));
}

// RPUSH

void LLRedisClient::rPushX(arg_str_ref key, arg_str_ref val, cb_t&& cb) {
  command2("RPUSHX %s %s", key, val, std::forward<cb_t>(cb));
}

// SADD

void LLRedisClient::save(cb_t&& cb) {
  command0("SAVE", std::forward<cb_t>(cb));
}

void LLRedisClient::sCard(arg_str_ref key, cb_t&& cb) {
  command1("SCARD %s", key, std::forward<cb_t>(cb));
}

// SCRIPT_*
// SDIFF
// SDIFFSTORE

void LLRedisClient::select(redis_signed_t dbNum, cb_t&& cb) {
  command1("SELECT %i", dbNum, std::forward<cb_t>(cb));
}

void LLRedisClient::set(arg_str_ref key, arg_str_ref val, cb_t&& cb) {
  command2("SET %s %s", key, val, std::forward<cb_t>(cb));
}

void LLRedisClient::set(arg_str_ref key, redis_signed_t val, cb_t&& cb) {
  command2("SET %s %i", key, val, std::forward<cb_t>(cb));
}

void LLRedisClient::setBit(arg_str_ref key, redis_signed_t offset,
    bool isSet, cb_t&& cb) {
  int bitVal = 0;
  if (isSet) {
    bitVal = 1;
  }
  command3("SETBIT %s %i %i", key, offset, bitVal, std::forward<cb_t>(cb));
}

void LLRedisClient::setEx(arg_str_ref key, redis_signed_t ttl, arg_str_ref val, cb_t&& cb) {
  command3("SETEX %s %i %s", key, ttl, val, std::forward<cb_t>(cb));
}

void LLRedisClient::setNX(arg_str_ref key, arg_str_ref val, cb_t&& cb) {
  command2("SETNX %s %s", key, val, std::forward<cb_t>(cb));
}

void LLRedisClient::setNX(arg_str_ref key, redis_signed_t val, cb_t&& cb) {
  command2("SETNX %s %i", key, val, std::forward<cb_t>(cb));
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

