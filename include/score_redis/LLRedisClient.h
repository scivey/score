#pragma once
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include "score/Try.h"
#include "score/Unit.h"
#include "score/Optional.h"
#include "score/func/Function.h"
#include "score_async/EventContext.h"
#include "score_redis/RedisDynamicResponse.h"

// #include "score/redis/LLRedisRequestContext.h"
// #include "score/redis/RedisSubscription.h"

struct redisAsyncContext;

namespace score { namespace redis {

struct LLRequestContext {
  using response_t = RedisDynamicResponse;
  using cb_t = func::Function<void, response_t>;
  cb_t callback;
  LLRequestContext(){}
  LLRequestContext(cb_t&& cb): callback(std::forward<cb_t>(cb)){}
  LLRequestContext(const cb_t& cb): callback(cb){}
};

class LLRedisClient: public std::enable_shared_from_this<LLRedisClient> {
 public:

  // using response_t = typename LLRedisRequestContext::response_t;
  using response_t = RedisDynamicResponse;
  using cb_t = func::Function<void, response_t>;
  using connect_result_t = score::Try<std::shared_ptr<LLRedisClient>>;
  using connect_cb_t = func::Function<void, connect_result_t>;
  using disconnect_cb_t = func::Function<void, score::Try<score::Unit>>;
  using string_t = std::string;
  using cmd_str_t = string_t;
  using arg_str_t = cmd_str_t;
  using cmd_str_ref = const cmd_str_t&;
  using arg_str_ref = const arg_str_t&;
  using redis_signed_t = int64_t;
  using arg_str_list = std::vector<arg_str_t>;
  using mset_list = std::vector<std::pair<arg_str_t, arg_str_t>>;
  // using subscription_t = RedisSubscription;
  // using subscription_try_t = score::Try<std::shared_ptr<subscription_t>>;
  // using subscription_handler_ptr_t = subscription_t::handler_ptr_t;
  using event_ctx_t = score::async::EventContext;
 protected:
  event_ctx_t *eventContext_ {nullptr};
  string_t host_;
  int port_ {0};
  struct redisAsyncContext *redisContext_ {nullptr};
  score::Optional<connect_cb_t> connectCallback_;
  score::Optional<disconnect_cb_t> disconnectCallback_;
  // std::weak_ptr<subscription_t> currentSubscription_;
  // connect_promise_t connectPromise_;
  // disconnect_promise_t disconnectPromise_;

  // not really for public use.
  LLRedisClient(event_ctx_t *ctx,
    const string_t& host, int port);

  LLRedisClient(const LLRedisClient &other) = delete;
  LLRedisClient& operator=(const LLRedisClient &other) = delete;

  void command0(cmd_str_ref cmd, cb_t&&);
  void command1(cmd_str_ref cmd, arg_str_ref arg, cb_t&&);
  void command2(cmd_str_ref cmd, arg_str_ref arg1,
      arg_str_ref arg2, cb_t&&);
  void command2(cmd_str_ref cmd, arg_str_ref arg1,
      redis_signed_t arg2, cb_t&&);

 public:

  LLRedisClient(LLRedisClient &&other);
  LLRedisClient& operator=(LLRedisClient &&other);

  static std::shared_ptr<LLRedisClient> createShared(event_ctx_t *ctx,
    const string_t &host, int port);

  void connect(connect_cb_t&&);
  void disconnect(disconnect_cb_t&&);

  void get(arg_str_ref, cb_t&&);
  void set(arg_str_ref, arg_str_ref, cb_t&&);
  void set(arg_str_ref, redis_signed_t, cb_t&&);

  // template<typename TCollection>
  // void mset(const TCollection &args, cb_t&& cb) {
  //   std::ostringstream oss;
  //   oss << "MSET";
  //   for (const auto &keyVal: args) {
  //     oss << " " << keyVal.first << " " << keyVal.second;
  //   }
  //   return command0(oss.str(), std::forward<cb_t>(cb));
  // }

  using mset_init_list = std::initializer_list<std::pair<arg_str_t, arg_str_t>>;
  // void mset(mset_init_list&& msetList, cb_t);


  // template<typename TCollection>
  // void mget(const TCollection &args, cb_t&& cb) {
  //   std::ostringstream oss;
  //   oss << "MGET";
  //   for (const auto &key: args) {
  //     oss << " " << key;
  //   }
  //   return command0(oss.str(), std::forward<cb_t>(cb));
  // }

  // using mget_init_list = std::initializer_list<arg_str_t>;

  // response_future_t mget(mget_init_list&& mgetList);
  // response_future_t exists(arg_str_ref);
  // response_future_t del(arg_str_ref);
  // response_future_t expire(arg_str_ref, redis_signed_t);
  // response_future_t setnx(arg_str_ref, arg_str_ref);
  // response_future_t setnx(arg_str_ref, redis_signed_t);
  // response_future_t getset(arg_str_ref, arg_str_ref);

  // response_future_t keys(arg_str_ref pattern);

  // response_future_t strlen(arg_str_ref);

  // response_future_t decr(arg_str_ref);
  // response_future_t decrby(arg_str_ref, redis_signed_t);
  // response_future_t incr(arg_str_ref key);
  // response_future_t incrby(arg_str_ref key, redis_signed_t);

  // response_future_t llen(arg_str_ref key);

  // subscription_try_t subscribe(subscription_handler_ptr_t, arg_str_ref);

 protected:
  // event handler methods called from the static handlers (because C)
  void handleConnected(int status);
  void handleCommandResponse(LLRequestContext *ctx, response_t&& data);
  void handleDisconnected(int status);
  void handleSubscriptionEvent(response_t&& data);

 public:
  // these are public because hiredis needs access to them.
  // they aren't really "public" public
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


