#pragma once
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include <atomic>
#include "score/macros/core.h"
#include "score/Unit.h"
#include "score_redis/RedisDynamicResponse.h"
#include "score_redis/RedisError.h"
#include "score/exceptions/ScoreError.h"

namespace score { namespace redis {

class LLRedisClient;

class LLRedisSubscription: public std::enable_shared_from_this<LLRedisSubscription> {
 public:

  #define X SCORE_DECLARE_EXCEPTION
    X(NotActive, SubscriptionError)
    X(AlreadyStopping, SubscriptionError)
  #undef X
  using message_t = RedisDynamicResponse;

  class EventHandler: public std::enable_shared_from_this<EventHandler> {
   public:
    using parent_t = LLRedisSubscription;
    friend class LLRedisSubscription;
   protected:
    parent_t *parent_ {nullptr};
    void setParent(parent_t *parent);
   public:
    parent_t* getParent() const;
    virtual void stop();
    virtual void onStarted() = 0;
    virtual void onMessage(message_t&& message) = 0;
    virtual void onStopped() = 0;
    virtual ~EventHandler() = default;
  };

  friend class LLRedisClient;

  using handler_ptr_t = std::shared_ptr<EventHandler>;
  using client_ptr_t = std::shared_ptr<LLRedisClient>;
 protected:
  client_ptr_t client_;
  handler_ptr_t handler_;
  std::atomic<bool> stopping_ {false};

  void updateHandlerParent();
  LLRedisSubscription(client_ptr_t, handler_ptr_t);
  void dispatchMessage(message_t&& message);
  static LLRedisSubscription* createNew(
    client_ptr_t, handler_ptr_t
  );
  static std::shared_ptr<LLRedisSubscription> createShared(
    client_ptr_t, handler_ptr_t
  );
  SCORE_DISABLE_COPY_AND_ASSIGN(LLRedisSubscription);
 public:
  score::Try<score::Unit> stop();
  LLRedisSubscription(LLRedisSubscription &&other);
  LLRedisSubscription& operator=(LLRedisSubscription &&other);
  bool isAlive() const;
};

}} // score::redis


