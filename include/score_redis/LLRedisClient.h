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
#include "score_redis/LLRedisSubscription.h"

struct redisAsyncContext;

namespace score { namespace redis {


namespace hiredis_adapter {
class LibeventRedisContext;
} // hiredis_adapter

class LLRedisClient: public std::enable_shared_from_this<LLRedisClient> {
 public:

  // using response_t = typename LLRedisRequestContext::response_t;
  using inner_response_t = RedisDynamicResponse;
  using response_t = score::Try<inner_response_t>;
  using cb_t = func::Function<void, response_t>;
  using connect_result_t = score::Try<score::Unit>;
  using disconnect_result_t = score::Try<score::Unit>;
  using connect_promise_t = async::futures::st::STPromise<connect_result_t>;
  using connect_future_t = typename connect_promise_t::future_t;
  using disconnect_promise_t = async::futures::st::STPromise<disconnect_result_t>;
  using disconnect_future_t = typename disconnect_promise_t::future_t;

  using string_t = std::string;
  using cmd_str_t = string_t;
  using arg_str_t = cmd_str_t;
  using cmd_str_ref = const cmd_str_t&;
  using arg_str_ref = const arg_str_t&;
  using redis_signed_t = int64_t;
  using redis_float_t = float;
  using arg_str_list = std::vector<arg_str_t>;
  using mset_list = std::vector<std::pair<arg_str_t, arg_str_t>>;
  using subscription_t = LLRedisSubscription;
  using subscription_try_t = score::Try<std::shared_ptr<subscription_t>>;
  using subscription_handler_ptr_t = subscription_t::handler_ptr_t;
  using event_ctx_t = score::async::EventContext;
  using string_init_list = std::initializer_list<arg_str_t>;
  using string_pair_init_list = std::initializer_list<std::pair<arg_str_t, arg_str_t>>;


  struct RequestContext {
    cb_t callback;
    RequestContext(cb_t&& cb);
  };

 protected:
  event_ctx_t *eventContext_ {nullptr};
  io::SocketAddr serverAddr_;
  struct redisAsyncContext *redisContext_ {nullptr};
  std::shared_ptr<hiredis_adapter::LibeventRedisContext> adapter_ {nullptr};
  connect_promise_t connectPromise_;
  disconnect_promise_t disconnectPromise_;

  std::weak_ptr<subscription_t> currentSubscription_;

  // not really for public use.
  LLRedisClient(event_ctx_t *ctx, io::SocketAddr&& serverAddr);

  LLRedisClient(const LLRedisClient &other) = delete;
  LLRedisClient& operator=(const LLRedisClient &other) = delete;

  void command0(cmd_str_ref cmd, cb_t&&);
  void command1(cmd_str_ref cmd, arg_str_ref arg, cb_t&&);
  void command1(cmd_str_ref cmd, redis_signed_t, cb_t&&);

  void command2(cmd_str_ref cmd, arg_str_ref arg1,
      arg_str_ref arg2, cb_t&&);
  void command2(cmd_str_ref cmd, arg_str_ref arg1,
      redis_signed_t arg2, cb_t&&);
  void command2(cmd_str_ref cmd, arg_str_ref arg1,
      redis_float_t arg2, cb_t&&);

  void command3(cmd_str_ref, arg_str_ref, arg_str_ref, arg_str_ref, cb_t&&);
  void command3(cmd_str_ref, arg_str_ref, redis_signed_t, redis_signed_t, cb_t&&);
  void command3(cmd_str_ref, arg_str_ref, redis_signed_t, arg_str_ref, cb_t&&);
  void command3(cmd_str_ref, arg_str_ref, arg_str_ref, redis_signed_t, cb_t&&);

 public:

  LLRedisClient(LLRedisClient &&other);
  LLRedisClient& operator=(LLRedisClient &&other);

  static LLRedisClient* createNew(event_ctx_t *, io::SocketAddr&&);

  connect_future_t connect();
  disconnect_future_t disconnect();


  void decr(arg_str_ref, cb_t&&);
  void decrBy(arg_str_ref, redis_signed_t, cb_t&&);
  void del(arg_str_ref, cb_t&&);
  void exec(cb_t&&);
  void echo(arg_str_ref, cb_t&&);
  void exists(arg_str_ref, cb_t&&);
  void expire(arg_str_ref, redis_signed_t, cb_t&&);
  void expireAt(arg_str_ref, redis_signed_t, cb_t&&);

  void flushAll(cb_t&&);
  void flushDB(cb_t&&);
  void get(arg_str_ref, cb_t&&);
  void getBit(arg_str_ref, redis_signed_t, cb_t&&);
  void getRange(arg_str_ref, redis_signed_t, redis_signed_t, cb_t&&);
  void getSet(arg_str_ref, arg_str_ref, cb_t&&);

  // missing: GEO commands
  // missing: HDEL
  void hExists(arg_str_ref, arg_str_ref, cb_t&&);
  void hGet(arg_str_ref, arg_str_ref, cb_t&&);
  void hGetAll(arg_str_ref, cb_t&&);
  void hIncrBy(arg_str_ref, arg_str_ref, redis_signed_t, cb_t&&);
  void hIncrByFloat(arg_str_ref, arg_str_ref, redis_float_t, cb_t&&);
  void hKeys(arg_str_ref, cb_t&&);


  void hMGet(arg_str_ref key, string_init_list&& fields, cb_t&&);

  template<typename TCollection>
  void hMGet(arg_str_ref key, const TCollection& fields, cb_t&& cb) {
    std::ostringstream oss;
    oss << "HMGET " << key;
    for (const auto &field: fields) {
      oss << " " << field;
    }
    command0(oss.str(), std::forward<cb_t>(cb));
  }

  void hMSet(arg_str_ref key, string_pair_init_list&& fieldVals, cb_t&&);

  template<typename TCollection>
  void hMSet(arg_str_ref key, const TCollection& fieldVals, cb_t&& cb) {
    std::ostringstream oss;
    oss << "HMSET " << key;
    for (const auto &fieldVal: fieldVals) {
      oss << " " << fieldVal.first << " " << fieldVal.second;
    }
    command0(oss.str(), std::forward<cb_t>(cb));
  }

  void hSet(arg_str_ref, arg_str_ref, arg_str_ref, cb_t&&);
  void hSetNX(arg_str_ref, arg_str_ref, arg_str_ref, cb_t&&);
  void hStrLen(arg_str_ref, arg_str_ref, cb_t&&);
  void hVals(arg_str_ref, cb_t&&);

  void incr(arg_str_ref key, cb_t&&);
  void incrBy(arg_str_ref key, redis_signed_t, cb_t&&);
  void incrByFloat(arg_str_ref key, redis_float_t, cb_t&&);
  void info(cb_t&&);
  void keys(arg_str_ref pattern, cb_t&&);

  // void lastSave(cb_t&&);
  void lIndex(arg_str_ref, redis_signed_t, cb_t&&);

  void lInsertBefore(arg_str_ref key, arg_str_ref pivot, arg_str_ref val, cb_t&&);
  void lInsertAfter(arg_str_ref key, arg_str_ref pivot, arg_str_ref val, cb_t&&);

  void lLen(arg_str_ref ref, cb_t&&);
  void lPop(arg_str_ref ref, cb_t&&);

  void lPush(arg_str_ref key, arg_str_ref val, cb_t&& cb);

  // sfinae here is to only match containers of strings
  template<typename TCollection,
    typename = decltype(std::declval<typename TCollection::value_type>().c_str())>
  void lPush(arg_str_ref key, const TCollection& collection, cb_t&& cb) {
    std::ostringstream oss;
    oss << "LPUSH " << key;
    for (const auto& item: collection) {
      oss << " " << item;
    }
    return command0(oss.str(), std::forward<cb_t>(cb));
  }

  void lPush(arg_str_ref key, string_init_list&& vals, cb_t&& cb);


  void lPushX(arg_str_ref, arg_str_ref, cb_t&&);
  void lRange(arg_str_ref, redis_signed_t, redis_signed_t, cb_t&&);
  void lRem(arg_str_ref key, redis_signed_t count, arg_str_ref value, cb_t&&);
  void lSet(arg_str_ref key, redis_signed_t idx, arg_str_ref value, cb_t&&);
  void lTrim(arg_str_ref key, redis_signed_t start, redis_signed_t stop, cb_t&&);

  template<typename TCollection>
  void mGet(const TCollection &args, cb_t&& cb) {
    std::ostringstream oss;
    oss << "MGET";
    for (const auto &key: args) {
      oss << " " << key;
    }
    return command0(oss.str(), std::forward<cb_t>(cb));
  }

  void mGet(string_init_list&& mgetList, cb_t&&);

  // missing: MIGRATE, MONITOR, MOVE

  template<typename TCollection>
  void mSet(const TCollection &args, cb_t&& cb) {
    std::ostringstream oss;
    oss << "MSET";
    for (const auto &keyVal: args) {
      oss << " " << keyVal.first << " " << keyVal.second;
    }
    auto cmd = oss.str();
    return command0(oss.str(), std::forward<cb_t>(cb));
  }

  void mSet(string_pair_init_list&& msetList, cb_t&&);

  // missing: MSETNX
  void multi(cb_t&&);

  void persist(arg_str_ref, cb_t&&);
  void pExpire(arg_str_ref, redis_signed_t, cb_t&&);
  void pExpireAt(arg_str_ref, redis_signed_t, cb_t&&);

  // missing: PFADD, PFCOUNT, PFMERGE

  void ping(arg_str_ref, cb_t&&);
  void pSetEx(arg_str_ref, redis_signed_t msec, cb_t&&);

  // missing: PSUBSCRIBE, PUBSUB

  void pTTL(arg_str_ref, cb_t&&);
  void publish(arg_str_ref channel, arg_str_ref msg, cb_t&&);
  // missing: PUNSUBSCRIBE, QUIT

  void randomKey(cb_t&&);
  void rename(arg_str_ref key, arg_str_ref newKey, cb_t&&);
  void renameNX(arg_str_ref key, arg_str_ref newKey, cb_t&&);
  // missing: RESTORE, ROLE

  void rPop(arg_str_ref key, cb_t&&);
  void rPopLPush(arg_str_ref source, arg_str_ref dest, cb_t&&);

  // missing: RPUSH
  void rPushX(arg_str_ref key, arg_str_ref value, cb_t&&);
  // missing: SADD
  void save(cb_t&&);
  void sCard(arg_str_ref, cb_t&&);

  // missing: SCRIPT_* commands

  // missing: SDIFF, SDIFFSTORE
  void select(redis_signed_t dbNum, cb_t&&);


  // SET has some options missing
  void set(arg_str_ref, arg_str_ref, cb_t&&);
  void set(arg_str_ref, redis_signed_t, cb_t&&);

  void setBit(arg_str_ref key, redis_signed_t offset, bool value, cb_t&&);
  void setEx(arg_str_ref key, redis_signed_t secs, arg_str_ref value, cb_t&&);

  void setNX(arg_str_ref, arg_str_ref, cb_t&&);
  void setNX(arg_str_ref, redis_signed_t, cb_t&&);

  // missing:
  //  SETRANGE
  //  SHUTDOWN
  //  SINTER
  //  SINTERSTORE
  //  SISMEMBER
  //  SLOWLOG
  //  SMEMBERS
  //  SMOVE
  //  SORT
  //  SPOP
  //  SRANDMEMBER
  //  SREM

  void strLen(arg_str_ref key, cb_t&&);
  // missing:
  //  SUNION
  //  SUNIONSTORE
  //  TIME
  //  TOUCH
  //  TTL
  //  TYPE
  //  UNSUBSCRIBE -> end





  subscription_try_t subscribe(arg_str_ref, subscription_handler_ptr_t);


 protected:
  // event handler methods called from the static handlers (because C)
  void handleConnected(int status);
  void handleCommandResponse(RequestContext *ctx, inner_response_t&& data);
  void handleDisconnected(int status);
  void handleSubscriptionEvent(inner_response_t&& data);
  void maybeCleanupContext();
 public:
  // these are public because hiredis needs access to them,
  // but they aren't really "public" public.
  // if you're wondering whether you count as "public",
  // you probably do.  unless you're me, in which case hi.
  static void hiredisConnectCallback(const redisAsyncContext*, int status);
  static void hiredisCommandCallback(redisAsyncContext*, void *reply, void *pdata);
  static void hiredisDisconnectCallback(const redisAsyncContext*, int status);
  static void hiredisSubscriptionCallback(redisAsyncContext*, void *reply, void *pdata);
  ~LLRedisClient();
};

namespace detail {
LLRedisClient* getClientFromContext(const redisAsyncContext* ctx);
}


}} // score::redis


