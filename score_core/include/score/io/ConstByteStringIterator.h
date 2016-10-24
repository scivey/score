#pragma once
#include <string>
#include <type_traits>
#include <iterator>

namespace score { namespace io {

template<typename TItem,
  typename TIgnore = typename std::enable_if<
    std::is_integral<TItem>::value>::type>
class ConstByteStringIterator: std::iterator<std::random_access_iterator_tag, TItem> {
 public:
  using parent_type = std::iterator<std::random_access_iterator_tag, TItem>;
  using iterator_category = typename parent_type::iterator_category;
  using value_type = typename parent_type::value_type;
  using pointer = typename parent_type::pointer;
  using reference = typename parent_type::reference;
  using difference_type = typename parent_type::difference_type;
 protected:
  const char *start_;
  const char *end_;
  size_t idx_;
 public:
  ConstByteStringIterator(const ConstByteStringIterator &other)
    : start_(other.start_), end_(other.end_), idx_(other.idx_) {}
  ConstByteStringIterator(const char *start, const char *end, size_t idx)
    : start_(start), end_(end), idx_(idx) {}
  ConstByteStringIterator(const std::string &text, size_t startIdx,
      size_t endIdx, size_t idx)
    : start_(text.data() + startIdx),
      end_(text.data() + endIdx),
      idx_(idx){}
  ConstByteStringIterator(const std::string &text, size_t idx)
    : start_(text.data()), end_(text.data() + text.size()), idx_(idx) {}

  TItem operator*() const {
    return (TItem) (*(start_ + idx_));
  }

  size_t rangeSize() const {
    auto startPtr = (uintptr_t) start_;
    auto endPtr = (uintptr_t) end_;
    if (endPtr < startPtr) {
      return 0;
    }
    return endPtr - startPtr;
  }

 protected:
  ConstByteStringIterator copyThis() {
    return ConstByteStringIterator(start_, end_, idx_);
  }

 public:
  ConstByteStringIterator& operator++() {
    if (idx_ <= rangeSize()) {
      idx_++;
    }
    return *this;
  }

  ConstByteStringIterator operator++(int) {
    auto result = copyThis();
    ++result;
    return result;
  }

  ConstByteStringIterator operator+(int x) {
    auto result = copyThis();
    result += x;
    return result;
  }

  ConstByteStringIterator operator-(int x) {
    auto result = copyThis();
    result -= x;
    return result;
  }

  ConstByteStringIterator& operator+=(int x) {
    auto rangeLimit = rangeSize() + 1;
    if ((idx_ + x) <= rangeLimit) {
      idx_ += x;
    } else {
      idx_ = rangeLimit;
    }
    return *this;
  }

  ConstByteStringIterator& operator-=(int x) {
    if (idx_ < x) {
      x = idx_;
    }
    idx_ -= x;
    return *this;
  }

  bool operator!=(const ConstByteStringIterator &other) const {
    return idx_ != other.idx_;
  }

  bool operator==(const ConstByteStringIterator &other) const {
    return idx_ == other.idx_;
  }

  bool operator<(const ConstByteStringIterator &other) const {
    return idx_ < other.idx_;
  }
};

}} // score::io
