#pragma once

#include "evs/events2/BaseEvent.h"
#include <glog/logging.h>

namespace evs { namespace events2 {

class DefaultEvent: public BaseEvent<DefaultEvent> {
 public:
  void onSignal() {
    LOG(INFO) << "onSignal";
  }
  void onReadable() {
    LOG(INFO) << "onReadable";
  }
  void onWritable() {
    LOG(INFO) << "onWritable";
  }
  void onTimeout() {
    LOG(INFO) << "onTimeout";
  }
};

}} // evs::events2