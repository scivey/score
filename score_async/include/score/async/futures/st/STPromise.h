#pragma once

#include <string>
#include <glog/logging.h>
#include <folly/Format.h>
#include "score/mem/st_shared_ptr.h"
#include "score_async/futures/st/traits.h"
#include "score_async/futures/st/STSharedState.h"
#include "score_async/futures/st/STFutureCore.h"

namespace score { namespace async { namespace futures { namespace st {

template<typename T> class STFuture;

template<typename T>
class STPromise {
 public:
  using state_t = STSharedState<T>;
  using state_ptr_t = score::mem::st_shared_ptr<state_t>;
  using future_core_t = STFutureCore<T>;
  using future_core_ptr_t = score::mem::st_shared_ptr<future_core_t>;
  using future_t = STFuture<T>;
  using ttraits = st_traits<T>;
  using error_t = typename ttraits::error_t;
  using value_type = typename ttraits::value_type;
 protected:
  state_ptr_t sharedState_ {nullptr};
  future_core_ptr_t futureCore_ {nullptr};

  state_ptr_t& getSharedState() {
    if (!sharedState_) {
      sharedState_ = score::mem::make_st_shared<state_t>();
    }
    return sharedState_;
  }
 public:
  static STPromise* createNew() {
    return new STPromise;
  }
  static STPromise create() {
    return STPromise{};
  }
  future_t getFuture();

  void setValue(value_type&& value) {
    getSharedState()->triggerValue(std::forward<value_type>(value));
  }
  void setValue(const value_type& value) {
    getSharedState()->triggerValue(value);
  }
  void setException(error_t&& err) {
    getSharedState()->triggerError(std::forward<error_t>(err));
  }
  void setException(const error_t& err) {
    getSharedState()->triggerError(err);
  }

  template<typename TError>
  void setException(const std::string& fmt) {
    setException(score::makeExceptionWrapper<TError>(fmt));
  }

  template<typename TError, typename T1, typename... Types>
  void setException(const std::string& fmt, const T1& arg1, Types&& ...args) {
    auto msg = folly::sformat(fmt, arg1, std::forward<Types>(args)...);
    auto asString = folly::to<std::string>(msg);
    setException(score::makeExceptionWrapper<TError>(asString));
  }

};

}}}} // score::async::futures::st

#include "score_async/futures/st/STFuture.h"

namespace score { namespace async { namespace futures { namespace st {


template<typename T>
STFuture<T> STPromise<T>::getFuture() {
  CHECK(!futureCore_);
  futureCore_ = score::mem::make_st_shared<future_core_t>();
  futureCore_->bindToState(getSharedState());
  return STFuture<T>(futureCore_);
}

}}}} // score::async::futures::st

