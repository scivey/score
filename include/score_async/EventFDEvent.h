#pragma once

#include <glog/logging.h>
#include "evs/events2/BaseEvent.h"
#include "evs/events2/BaseEvent.h"

#include "evs/events2/VoidCallback.h"

namespace evs { namespace events2 {


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

}} // evs::events2
