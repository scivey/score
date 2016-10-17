#pragma once

#include <glog/logging.h>
#include "score_async/BaseEvent.h"
#include "score_async/VoidCallback.h"

namespace score { namespace async {

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

}} // score::async
