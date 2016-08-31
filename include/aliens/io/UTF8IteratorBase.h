#pragma once
#include "aliens/vendored/utf8/utf8.h"
#include <string>
#include <iterator>

namespace aliens { namespace io {

template<typename TSubType, typename TValue>
class UTF8IteratorBase: std::iterator<std::forward_iterator_tag, TValue> {
 public:
  using subtype = TSubType;
  using iter_type = std::iterator<std::forward_iterator_tag, TValue>;
  using pointer = typename iter_type::pointer;
  using value_type = typename iter_type::value_type;
  using reference = typename iter_type::reference;
  using iterator_category = std::forward_iterator_tag;
  using difference_type = typename iter_type::difference_type;

 protected:
  TSubType* asSub() {
    return static_cast<TSubType*>(this);
  }

  const TSubType* asSub() const {
    return static_cast<const TSubType*>(this);
  }

  const char *position_;
  const char *endPos_;
  size_t distanceFromStart_;

 public:
  UTF8IteratorBase(const char *pos, const char *endPos)
    : position_(pos), endPos_(endPos) {}
  UTF8IteratorBase(const char *pos, const char *endPos, size_t dist)
    : position_(pos), endPos_(endPos), distanceFromStart_(dist) {}

  TValue operator*() const {
    return asSub()->dereference();
  }

  TSubType& operator=(const TSubType &other) {
    position_ = other.position_;
    endPos_ = other.endPos_;
    distanceFromStart_ = other.distanceFromStart_;
    return *asSub();
  }

  TSubType& operator++() {
    const char *previous = position_;
    aliens::vendored::utf8::unchecked::next(position_);
    uintptr_t prevPtr = (uintptr_t) previous;
    uintptr_t currentPtr = (uintptr_t) position_;
    size_t offset = currentPtr - prevPtr;
    distanceFromStart_ += offset;
    return *asSub();
  }

  TSubType operator++(int) {
    TSubType result(position_, endPos_, distanceFromStart_);
    ++result;
    return result;
  }

  bool operator==(const TSubType &other) const {
    return position_ == other.position_;
  }

  bool operator!=(const TSubType &other) const {
    return position_ != other.position_;
  }

  bool operator<(const TSubType &other) const {
    return position_ < other.position_;
  }

  bool good() const {
    return ((uintptr_t) position_) < ((uintptr_t) endPos_);
  }

  operator bool() const {
    return good();
  }
};

}} // aliens::io
