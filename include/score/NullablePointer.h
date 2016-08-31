#pragma once
#include "score/exceptions/macros.h"

namespace score {

/*
*  NullablePointer represents a pointer which may or may not be null.
*  It should act just like a regular (bare) pointer under ordinary
*  circumstances.
*
*  Its purposes are to:
*    - Make possibly-null-returning functions self-documenting.
*    - Enable more useful assertions in debug builds.
*
*  NB:
*    - NullablePointer *does not own* its (potentially null) contained pointer.
*    - NullablePointer does not do any internal synchronization or locking -
*      that's up to the client.
*
*  Why not use score::Maybe<T> for this?
*    - A maybe-null pointer is a very simple thing.  Maybe<T> handles the more
*      general case of any possibly-nonexistent value type, and so has extra
*      complexity.
*    - The useful debug assertions + annotations for the two uses cases are
*      similar, but still different.
*    - Maybe<T> isn't assumed to be copy-constructible in the general case,
*      while a pointer type is.
*/


template<typename T>
class NullablePointer {
 public:
  using value_type = T;
  using pointer_type = T*;
 protected:
  T* value_ {nullptr};
 public:
  NullablePointer(){}
  NullablePointer(T* value): value_(value) {}
  void assign(T *value) {
    value_ = value;
  }
  void assign(const NullablePointer &other) {
    value_ = other.value_;
  }
  void reset() {
    value_ = nullptr;
  }
  void reset(T *value) {
    value_ = value;
  }
  operator bool() const {
    return hasValue();
  }
  bool hasValue() const {
    return !!value_;
  }
  T& value() {
    SDCHECK(hasValue());
    return *value_;
  }
  const T& value() const {
    SDCHECK(hasValue());
    return *value_;
  }
  T* get() {
    SDCHECK(hasValue());
    return value_;
  }
  const T* get() const {
    SDCHECK(hasValue());
    return value_;
  }
  bool isEmpty() const {
    return !hasValue();
  }
  T* operator->() {
    return get();
  }
  const T* operator->() const {
    return get();
  }
  T& operator*() {
    return value();
  }
  const T& operator*() const {
    return value();
  }
};

} // score
