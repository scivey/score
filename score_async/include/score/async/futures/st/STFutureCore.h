#pragma once

#include <string>
#include <glog/logging.h>
#include "score/mem/st_shared_ptr.h"
#include "score/async/futures/st/traits.h"
#include "score/async/futures/st/STSharedState.h"

namespace score { namespace async { namespace futures { namespace st {

template<typename T>
class STFutureCore: public score::mem::enable_st_shared_from_this<STFutureCore<T>> {
 public:
  using ttraits = st_traits<T>;
  using value_type = typename ttraits::value_type;
  using error_t = typename ttraits::error_t;
  using state_t = STSharedState<T>;
  using state_ptr_t = score::mem::st_shared_ptr<state_t>;
  using value_cb_t = typename ttraits::value_cb_t;
  using error_cb_t = typename ttraits::error_cb_t;
 protected:
  state_ptr_t sharedState_ {nullptr};
  value_cb_t onValue_;
  error_cb_t onError_;

  void triggerValue(value_type value) {
    onValue_(value);
  }

  void triggerError(error_t err) {
    onError_(err);
  }

  void bindValueCallback() {
    CHECK(sharedState_ && onValue_);
    std::function<void(value_type)> handler([this](value_type val) {
      this->triggerValue(std::move(val));
    });
    sharedState_->setValueCallback(std::move(handler));
  }

  void bindErrorCallback() {
    CHECK(sharedState_ && onError_);
    std::function<void(error_t)> handler([this](error_t err) {
      this->triggerError(std::move(err));
    });
    sharedState_->setErrorCallback(std::move(handler));
  }

 public:
  void bindToState(state_ptr_t sharedState) {
    sharedState_ = sharedState;
    if (onError_) {
      bindErrorCallback();
    }
    if (onValue_) {
      bindValueCallback();
    }
  }

  void setErrorCallback(error_cb_t onErr) {
    CHECK(!onError_);
    onError_ = onErr;
    if (sharedState_) {
      bindErrorCallback();
    }
  }
  void setValueCallback(value_cb_t onVal) {
    CHECK(!onValue_);
    onValue_ = onVal;
    if (sharedState_) {
      bindValueCallback();
    }
  }
};

}}}} // score::async::futures::st
