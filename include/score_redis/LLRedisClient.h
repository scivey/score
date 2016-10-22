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

class LLRedisClient  {
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
  using arg_str_list = std::vector<arg_str_t>;
  using mset_list = std::vector<std::pair<arg_str_t, arg_str_t>>;
  // using subscription_t = RedisSubscription;
  // using subscription_try_t = score::Try<std::shared_ptr<subscription_t>>;
  // using subscription_handler_ptr_t = subscription_t::handler_ptr_t;
  using event_ctx_t = score::async::EventContext;

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

  // std::weak_ptr<subscription_t> currentSubscription_;

  // not really for public use.
  LLRedisClient(event_ctx_t *ctx, io::SocketAddr&& serverAddr);

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

  static LLRedisClient* createNew(event_ctx_t *, io::SocketAddr&&);

  connect_future_t connect();
  disconnect_future_t disconnect();

  void get(arg_str_ref, cb_t&&);
  void set(arg_str_ref, arg_str_ref, cb_t&&);
  void set(arg_str_ref, redis_signed_t, cb_t&&);

  template<typename TCollection>
  void mset(const TCollection &args, cb_t&& cb) {
    std::ostringstream oss;
    oss << "MSET";
    for (const auto &keyVal: args) {
      oss << " " << keyVal.first << " " << keyVal.second;
    }
    auto cmd = oss.str();
    return command0(oss.str(), std::forward<cb_t>(cb));
  }

  using mset_init_list = std::initializer_list<std::pair<arg_str_t, arg_str_t>>;
  void mset(mset_init_list&& msetList, cb_t&&);


  template<typename TCollection>
  void mget(const TCollection &args, cb_t&& cb) {
    std::ostringstream oss;
    oss << "MGET";
    for (const auto &key: args) {
      oss << " " << key;
    }
    return command0(oss.str(), std::forward<cb_t>(cb));
  }

  using mget_init_list = std::initializer_list<arg_str_t>;

  void mget(mget_init_list&& mgetList, cb_t&&);
  void exists(arg_str_ref);
  void del(arg_str_ref, cb_t&&);
  void expire(arg_str_ref, redis_signed_t, cb_t&&);
  void setnx(arg_str_ref, arg_str_ref, cb_t&&);
  void setnx(arg_str_ref, redis_signed_t, cb_t&&);
  void getset(arg_str_ref, arg_str_ref, cb_t&&);

  void keys(arg_str_ref pattern, cb_t&&);
  void strlen(arg_str_ref, cb_t&&);
  void decr(arg_str_ref, cb_t&&);
  void decrby(arg_str_ref, redis_signed_t, cb_t&&);
  void incr(arg_str_ref key, cb_t&&);
  void incrby(arg_str_ref key, redis_signed_t, cb_t&&);
  void llen(arg_str_ref key, cb_t&&);
  // subscription_try_t subscribe(subscription_handler_ptr_t, arg_str_ref);

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


