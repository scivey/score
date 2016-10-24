#include "score/async/CallbackEvent.h"
#include <glog/logging.h>

namespace score { namespace async {

void CallbackEvent::onReadable() {
  if (readCallback_) {
    readCallback_();
  } else {
    LOG(INFO) << "onReadable: no readCallback registered.";
  }
}
void CallbackEvent::onWritable() {
  if (writeCallback_) {
    writeCallback_();
  } else {
    LOG(INFO) << "onWritable: no writeCallback registered.";
  }
}
void CallbackEvent::onTimeout() {
  if (timeoutCallback_) {
    timeoutCallback_();
  } else {
    LOG(INFO) << "onTimeout: no timeoutCallback registered.";
  }
}

void CallbackEvent::onSignal() {}

}} // score::async
