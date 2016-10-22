#pragma once
#include <event.h>
#include <utility>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>

namespace score { namespace redis {
class LLRedisClient;
}}

namespace score { namespace redis { namespace hiredis_adapter {

// this class exists to manage lifetime of the InnerContext,
// which is used by the (mostly unchanged) hiredis libevent
// adapter code
class LibeventRedisContext: public std::enable_shared_from_this<LibeventRedisContext> {
 public:
  struct InnerContext {
    redisAsyncContext *redisContext {nullptr};
    LLRedisClient *clientPtr {nullptr};
    struct event readEvent, writeEvent;
  };
 protected:
  InnerContext innerContext_;
 public:
  InnerContext* getInnerContext() {
    return &innerContext_;
  }
};

}}} // score::redis::hiredis_adapter
