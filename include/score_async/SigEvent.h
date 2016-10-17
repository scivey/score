#pragma once

#include <glog/logging.h>
#include "evs/events2/BaseEvent.h"
#include "evs/events2/VoidCallback.h"

namespace evs { namespace events2 {

class SigEvent: public BaseEvent<SigEvent> {
 protected:
  VoidCallback callback_;
 public:
  void onSignal() {
    CHECK(!!callback_);
    callback_();
  }
  template<typename TCallable>
  void setHandler(TCallable&& callable) {
    callback_ = std::forward<TCallable>(callable);
  }
  void onReadable() {}
  void onWritable() {}
  void onTimeout() {}
};

}} // evs::events2
