#pragma once

#include <glog/logging.h>
#include "score_async/BaseEvent.h"
#include "score_async/BaseEvent.h"

#include "score_async/VoidCallback.h"

namespace score { namespace async {


class EventFDEvent: public BaseEvent<EventFDEvent> {
 protected:
  VoidCallback readCallback_;
 public:

  template<typename TCallable>
  void setReadHandler(TCallable&& callable) {
    readCallback_ = std::forward<TCallable>(callable);
  }

  void onReadable() {
    DCHECK(!!readCallback_);
    readCallback_();
  }

  void onWritable() {}
  void onTimeout() {}
  void onSignal() {}
};

}} // score::async
