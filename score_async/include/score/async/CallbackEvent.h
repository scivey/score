#pragma once
#include <sstream>
#include <string>
#include <cstdio>
#include <glog/logging.h>
#include "score/async/BaseEvent.h"
#include "score/func/Function.h"

namespace score { namespace async {

class CallbackEvent: public BaseEvent<CallbackEvent> {
 public:
  using string_t = std::string;
  using event_cb_t = score::func::Function<void>;

 protected:
  event_cb_t readCallback_;
  event_cb_t writeCallback_;
  event_cb_t timeoutCallback_;

 public:
  void onReadable();
  void onWritable();
  void onTimeout();
  void onSignal();

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
};


}} // score::async
