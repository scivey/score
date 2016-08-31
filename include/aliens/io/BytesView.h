#pragma once
#include <string>
#include "aliens/io/ConstByteStringIterator.h"
#include "aliens/io/Scstring.h"
#include "aliens/io/UTF8View.h"

namespace aliens { namespace io {

template<typename TChar>
class BytesView {
 public:
  using iterator = ConstByteStringIterator<TChar>;
  using value_type = TChar;
  using size_type = size_t;
 protected:
  const value_type *start_;
  const value_type *end_;
 public:
  BytesView(const value_type *start, const value_type *end)
    : start_(start), end_(end) {}

  BytesView(const value_type *start, size_type offset)
    : start_(start), end_(start + offset) {}

  BytesView(const std::string &text, size_type start, size_type end)
    : start_(text.data() + start), end_(text.data() + end) {}

  BytesView(const std::string &text)
    : start_(text.data()), end_(text.data() + text.size()) {}

  size_type size() const {
    auto startPtr = (uintptr_t) start_;
    auto endPtr = (uintptr_t) end_;
    if (endPtr <= startPtr) {
      return 0;
    }
    return endPtr - startPtr;
  }

  iterator begin() const {
    return iterator(start_, end_, 0);
  }

  iterator cbegin() const {
    return iterator(start_, end_, 0);
  }

  iterator end() const {
    return iterator(start_, end_, size());
  }

  iterator cend() const {
    return iterator(start_, end_, size());
  }

  value_type at(size_type idx) const {
    auto ptr = start_;
    ptr += idx;
    return *ptr;
  }

  value_type operator[](size_type idx) const {
    return at(idx);
  }

  const value_type* data() const {
    return start_;
  }

  const value_type* c_str() const {
    return start_;
  }

  template<typename TStr>
  void appendTo(TStr &aStr) {
    aStr.append(data(), size());
  }

  UTF8View asUTF8View() const {
    return UTF8View(start_, end_);
  }
};

}} // aliens::ios
