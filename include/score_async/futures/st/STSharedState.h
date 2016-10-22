#pragma once

#include <string>
#include <glog/logging.h>
#include "score/mem/st_shared_ptr.h"
#include "score_async/futures/st/traits.h"

namespace score { namespace async { namespace futures { namespace st {

template<typename T>
class STSharedState: public score::mem::enable_st_shared_from_this<STSharedState<T>> {
 public:
  using ttraits = st_traits<T>;
  using value_type = typename ttraits::value_type;
  using error_t = typename ttraits::error_t;
  using value_cb_t = typename ttraits::value_cb_t;
  using error_cb_t = typename ttraits::error_cb_t;
  using value_option_t = typename ttraits::value_option_t;
  using error_option_t = typename ttraits::error_option_t;
  enum class State {
    PENDING, VALUE, ERROR
  };

  std::string stringOfState(State state) {
    switch(state) {
      case State::PENDING: return "State::PENDING";
      case State::VALUE: return "State::VALUE";
      case State::ERROR: return "State::ERROR";
      default:
        return "UNKNOWN";
    }
  }
 protected:
  State state_ {State::PENDING};
  value_cb_t onValue_;
  error_cb_t onError_;
  value_option_t valueOption_;
  error_option_t errorOption_;

  void maybeInvokeValue() {
    if (state_ == State::VALUE && onValue_) {
      onValue_(std::move(valueOption_.value()));
    }
  }

  void transitionToValue() {
    CHECK(valueOption_.hasValue());
    CHECK(state_ == State::PENDING) << "actual state: " << stringOfState(state_);
    state_ = State::VALUE;
    maybeInvokeValue();
  }

  void maybeInvokeError() {
    if (state_ == State::ERROR && onError_) {
      onError_(std::move(errorOption_.value()));
    }
  }

  void transitionToError() {
    CHECK(errorOption_.hasValue());
    CHECK(state_ == State::PENDING);
    state_ = State::ERROR;
    maybeInvokeError();
  }


 public:
  void triggerValue(const value_type& val) {
    valueOption_.assign(val);
    transitionToValue();
  }

  void triggerValue(value_type&& val) {
    valueOption_.assign(std::forward<value_type>(val));
    transitionToValue();
  }

  void triggerError(const error_t& err) {
    errorOption_.assign(err);
    transitionToError();
  }

  void triggerError(error_t&& err) {
    errorOption_.assign(std::forward<error_t>(err));
    transitionToError();
  }

  void setValueCallback(const value_cb_t& cb) {
    onValue_ = cb;
    maybeInvokeValue();
  }

  void setValueCallback(value_cb_t&& cb) {
    onValue_ = std::move(cb);
    maybeInvokeValue();
  }

  void setErrorCallback(const error_cb_t& cb) {
    onError_ = cb;
    maybeInvokeError();
  }
  void setErrorCallback(error_cb_t&& cb) {
    onError_ = std::move(cb);
    maybeInvokeError();
  }

};

}}}} // score::async::futures::st
