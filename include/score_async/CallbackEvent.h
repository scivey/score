#pragma once
#include <sstream>
#include <string>
#include <cstdio>
#include <folly/Format.h>
#include <glog/logging.h>
#include "evs/events2/BaseEvent.h"
#include "evs/events2/VoidCallback.h"

namespace evs { namespace events2 {

class CallbackEvent: public events2::BaseEvent<CallbackEvent> {
 public:
  using string_t = std::string;
  using event_cb_t = events2::VoidCallback;

 protected:
  event_cb_t readCallback_;
  event_cb_t writeCallback_;
  event_cb_t timeoutCallback_;

 public:
  void onReadable() {
    if (readCallback_) {
      readCallback_();
    } else {
      LOG(INFO) << "onReadable: no readCallback registered.";
    }
  }
  void onWritable() {
    if (writeCallback_) {
      writeCallback_();
    } else {
      LOG(INFO) << "onWritable: no writeCallback registered.";
    }
  }
  void onTimeout() {
    if (timeoutCallback_) {
      timeoutCallback_();
    } else {
      LOG(INFO) << "onTimeout: no timeoutCallback registered.";
    }
  }

  template<typename TCallable>
  void setReadHandler(TCallable&& callable) {
    readCallback_ = std::forward<TCallable>(callable);
  }
  template<typename TCallable>
  void setReadHandler(const TCallable& callable) {
    readCallback_ = callable;
  }
  template<typename TCallable>
  void setWriteHandler(TCallable&& callable) {
    writeCallback_ = std::forward<TCallable>(callable);
  }
  template<typename TCallable>
  void setWriteHandler(const TCallable& callable) {
    writeCallback_ = callable;
  }
  template<typename TCallable>
  void setTimeoutHandler(TCallable&& callable) {
    timeoutCallback_ = std::forward<TCallable>(callable);
  }
  template<typename TCallable>
  void setTimeoutHandler(const TCallable& callable) {
    timeoutCallback_ = callable;
  }
  void onSignal() {}
};


}} // evs::ev2prog
