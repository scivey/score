#pragma once

#include <glog/logging.h>
#include "score_async/BaseEvent.h"
#include "score_async/VoidCallback.h"

namespace score { namespace async {

class SignalEvent: public BaseEvent<SignalEvent> {
 public:
  using cb_t = VoidCallback;
 protected:
  cb_t callback_;
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

}} // score::async