#pragma once
#include <functional>
#include <event2/event.h>
#include <glog/logging.h>
#include <folly/futures/Try.h>
#include "evs/util/util.h"
#include "evs/events/TimerSettings.h"
#include "evs/events2/EvBase.h"
#include "evs/events2/EventError.h"
#include "evs/folly_util/try_helpers.h"

namespace evs { namespace events2 {


template<typename T>
class BaseEvent {
 public:
  using event_t = struct event;
  using event_ptr_t = typename evs::util::unique_destructor_ptr<event_t>::type;
  using fd_t = evutil_socket_t;
 protected:
  event_ptr_t event_ {nullptr};
  T* getThis() {
    return (T*) this;
  }
 public:
  bool good() const {
    return !!event_;
  }
  operator bool() const {
    return good();
  }
  event_t* getEvent() {
    DCHECK(good());
    return event_.get();
  }
  folly::Try<fd_t> getFD() {
    if (!good()) {
      return folly_util::makeTryFailure<fd_t, EventError>(
        "Invalid event."
      );
    }
    fd_t result = event_get_fd(event_.get());
    if (result < 0) {
      return folly_util::makeTryFailure<fd_t, EventError>(
        "Valid event, but invalid file descriptor: {}", result
      );
    }
    return folly_util::makeTrySuccess<fd_t>(result);
  }
  static T* createNewEvent(EvBase *base, fd_t fd, short what) {
    auto result = new T;
    auto evPtr = event_new(base->getBase(), fd, what, T::libeventCallback, (void*) result);
    result->event_ = evs::util::asDestructorPtr<event_t>(evPtr, event_free);
    return result;
  }
  static T* createNewSignalEvent(EvBase *base, int signum) {
    return createNewEvent(
      base, signum, EV_SIGNAL|EV_PERSIST
    );
  }
  static T* createNewTimerEvent(EvBase *base, const events::TimerSettings &settings) {
    auto evt = createNewEvent(
      base, -1, EV_PERSIST
    );
    evt->add(settings);
    return evt;
  }
  void doOnReadable() {
    getThis()->onReadable();
  }
  void doOnWritable() {
    getThis()->onWritable();
  }
  void doOnSignal() {
    getThis()->onSignal();
  }
  void doOnTimeout() {
    getThis()->onTimeout();
  }
  static void libeventCallback(evutil_socket_t sockFd, short what, void *arg) {
    auto ctx = (T*) arg;
    if (what & EV_READ) {
      ctx->doOnReadable();
    }
    if (what & EV_WRITE) {
      ctx->doOnWritable();
    }
    if (what & EV_SIGNAL) {
      ctx->doOnSignal();
    }
    if (what & EV_TIMEOUT) {
      ctx->doOnTimeout();
    }
  }
  void add() {
    event_add(event_.get(), nullptr);
  }
  void add(const events::TimerSettings &timeout) {
    timeval timeVal = timeout.toTimeVal();
    add(&timeVal);
  }
  void add(timeval *timeout) {
    event_add(event_.get(), timeout);
  }
  void del() {
    DCHECK(good());
    CHECK(event_del(event_.get()) == 0);
  }
};

}} // evs::events2
