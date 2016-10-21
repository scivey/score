#pragma once

#include <glog/logging.h>
#include "score_async/BaseEvent.h"
#include "score/func/Function.h"

namespace score { namespace async {


class TimerEvent: public BaseEvent<TimerEvent> {
 protected:
  func::Function<void> callback_;
 public:
  template<typename TCallable>
  void setHandler(TCallable&& callable) {
    callback_ = std::forward<TCallable>(callable);
  }
  void onReadable() {}
  void onWritable() {}
  void onSignal() {}
  void onTimeout() {
    CHECK(!!callback_);
    callback_();
  }
  static TimerEvent* createNew(EvBase *base, bool persist = true) {
    int flags = 0;
    if (persist) {
      flags |= EV_PERSIST;
    }
    return createNewEvent(
      base, -1, flags
    );
  }
  bool isPending() {
    return event_pending(getEvent(), EV_TIMEOUT, nullptr);
  }
};


}} // score::async
