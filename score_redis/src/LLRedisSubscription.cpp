#include "score/redis/LLRedisSubscription.h"
#include "score/redis/LLRedisClient.h"

using namespace std;

namespace score { namespace redis {

using handler_ptr_t = typename LLRedisSubscription::handler_ptr_t;
using client_ptr_t = typename LLRedisSubscription::client_ptr_t;
using message_t = typename LLRedisSubscription::message_t;

LLRedisSubscription::LLRedisSubscription(client_ptr_t client,
    handler_ptr_t handler)
  : client_(client), handler_(std::forward<handler_ptr_t>(handler)) {
  updateHandlerParent();
}

LLRedisSubscription::LLRedisSubscription(LLRedisSubscription &&other)
    : client_(std::move(other.client_)), handler_(std::move(other.handler_)) {
  stopping_.store(other.stopping_.load());
  updateHandlerParent();
}

LLRedisSubscription& LLRedisSubscription::operator=(LLRedisSubscription &&other) {
  std::swap(client_, other.client_);
  std::swap(handler_, other.handler_);

  // this isn't (and isn't really intended to be) atomic.
  // If you move-assign to this and also try to call `.stop()` on `other`
  // from another thread, you're gonna have a bad time.
  // If surprises you, you're gonna be having a lot of bad times.
  bool selfStopping = stopping_.load();
  stopping_.store(other.stopping_.load());
  other.stopping_.store(selfStopping);
  updateHandlerParent();
  other.updateHandlerParent();
  return *this;
}

bool LLRedisSubscription::isAlive() const {
  return !!handler_ && !stopping_.load(std::memory_order_acquire);
}

Try<Unit> LLRedisSubscription::stop() {
  // if there's a race with another thread, the returned `Try` will have
  // one of two exceptions: `NotActive` or `AlreadyStopping`.
  // these are both subclasses of `score::redis::SubscriptionError`.
  //
  // `NotActive` indicates that we found the handler set to null
  // before even entering the CAS loop. This may or may not indicate a race.
  // It would also be returned if the subscription weren't active to begin with.
  //
  // `AlreadyStopping` means the handler was truthy when we started
  // trying to stop, but failed in our CAS loop.  This always indicates
  // a race with another thread.

  if (!handler_) {
    return util::makeTryFailure<Unit, LLRedisSubscription::NotActive>(
      "Subscription is inactive."
    );
  }
  bool failed = false;
  for (;;) {
    // memory_order_acquire is sufficient here because
    // we aren't going to act on the result until attempting a CAS
    if (stopping_.load(std::memory_order_acquire)) {
      failed = true;
      break;
    }
    bool expected = false;
    bool desired = true;
    if (stopping_.compare_exchange_strong(expected, desired)) {
      break;
    }
  }
  if (failed) {
    return util::makeTryFailure<Unit, LLRedisSubscription::AlreadyStopping>(
      "The subscription is already shutting down."
    );
  }
  return util::makeTrySuccess<Unit>();
}

void LLRedisSubscription::updateHandlerParent() {
  if (handler_) {
    handler_->setParent(this);
  }
}

void LLRedisSubscription::EventHandler::setParent(LLRedisSubscription *parent) {
  parent_ = parent;
}

LLRedisSubscription* LLRedisSubscription::EventHandler::getParent() const {
  return parent_;
}

void LLRedisSubscription::dispatchMessage(RedisDynamicResponse&& message) {
  DCHECK(!!handler_);
  if (!stopping_.load(std::memory_order_relaxed)) {
    handler_->onMessage(std::forward<RedisDynamicResponse>(message));
  }
}

void LLRedisSubscription::EventHandler::stop() {
  auto parent = getParent();
  DCHECK(!!parent);
  auto result = parent->stop();
  if (result.hasException()) {
    // since this means the subscription is in a stopped state anyway,
    // we don't currently propagate failure to the EventHandler.
    LOG(INFO) << "unexpected exception on LLRedisSubscription::EventHandler::stop()"
              << ": '" << result.exception().what() << "'";
  }
}

shared_ptr<LLRedisSubscription> LLRedisSubscription::createShared(
    client_ptr_t client, handler_ptr_t handler) {
  return std::shared_ptr<LLRedisSubscription> {
    createNew(client, std::forward<handler_ptr_t>(handler))
  };
}

LLRedisSubscription* LLRedisSubscription::createNew(
    client_ptr_t client, handler_ptr_t handler) {
  return new LLRedisSubscription {client, std::forward<handler_ptr_t>(handler) };
}

}} // score::redis


