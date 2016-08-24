#pragma once
#include <glog/logging.h>

namespace aliens {


template<typename T>
class Maybe {
 public:
  enum class State {
    HAS_VALUE, EMPTY
  };
  using value_type = T;
 protected:
  State state_ {State::EMPTY};
  union {
    T value_;
  };
  Maybe(const Maybe&) = delete;
  Maybe& operator=(const Maybe&) = delete;
  void maybeDelete() {
    if (hasValue()) {
      value_.~T();
      state_ = State::EMPTY;
    }
  }
 public:
  Maybe(){}
  Maybe(Maybe &&other) {
    if (other.hasValue()) {
      new (&value_) T(std::move(other.value_));
      other.value_.~T();
    }
    state_ = other.state_;
    other.state_ = State::EMPTY;
  }
  Maybe& operator=(Maybe &&other) {
    maybeDelete();
    if (other.hasValue()) {
      new (&value_) T(std::move(other.value_));
      other.value_.~T();
    }
    state_ = other.state_;
    other.state_ = State::EMPTY;
    return *this;
  }
  Maybe(T &&value) {
    new (&value_) T(std::move(value));
    state_ = State::HAS_VALUE;
  }
  Maybe(const T &value) {
    new (&value_) T(value);
    state_ = State::HAS_VALUE;
  }
  void assign(T &&value) {
    maybeDelete();
    new (&value_) T(std::move(value));
    state_ = State::HAS_VALUE;
  }
  void assign(const T &value) {
    maybeDelete();
    new (&value_) T(value);
    state_ = State::HAS_VALUE;
  }
  void assign(Maybe &&other) {
    *this = std::move(other);
  }
  operator bool() const {
    return hasValue();
  }
  bool hasValue() const {
    return state_ == State::HAS_VALUE;
  }
  T& value() {
    CHECK(hasValue());
    return value_;
  }
  const T& value() const {
    CHECK(hasValue());
    return value_;
  }
  bool isEmpty() const {
    return !hasValue();
  }
};

} // aliens
