#pragma once

#include "score_async/BaseEvent.h"
#include <glog/logging.h>

namespace score { namespace async {

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

}} // score::async