#pragma once
// adapted from re2::StringPiece
// the detailed parts (find, compare, copy, etc)
// are copied verbatim.

#include <string>
#include <iterator>
#include <cstring>
#include <type_traits>
#include <glog/logging.h>

namespace score { namespace io {

class ByteStringPiece {
 public:
  using value_type = char;
  using size_type = size_t;
  using length_type = size_type;

 protected:
  const value_type *start_ {nullptr};
  size_type length_ {0};
 public:
  ByteStringPiece(const value_type *start, size_type length)
    : start_(start), length_(length) {}

  ByteStringPiece(const value_type *start): start_(start) {
    if (start_ != nullptr) {
      length_ = strlen(start_);
    }
  }

  template<typename TString,
    typename = decltype(std::declval<TString>().c_str()),
    typename = decltype(std::declval<TString>().size())>
  ByteStringPiece(const TString& text)
    : start_(text.c_str()), length_(text.size()) {}

  template<typename TString,
    typename = decltype(std::declval<TString>().c_str()),
    typename = decltype(std::declval<TString>().size())>
  ByteStringPiece(const TString& text, size_type length)
    : start_(text.c_str()), length_(length) {
    DCHECK(length_ <= text.size());
  }

  size_type size() const {
    return length_;
  }

  size_type length() const {
    return length_;
  }

  bool empty() const {
    return length_ == 0;
  }

  void clear() {
    start_ = nullptr;
    length_ = 0;
  }

  void set(const value_type *data, size_type len) {
    start_ = data;
    length_ = len;
  }

  void set(const value_type *data) {
    start_ = data;
    if (start_ == nullptr) {
      length_ = 0;
    } else {
      length_ = strlen(start_);
    }
  }

  void set(const void* data, size_type len) {
    start_ = reinterpret_cast<const value_type*>(data);
    length_ = len;
  }

  value_type at(size_type idx) const {
    return start_[idx];
  }

  value_type operator[](size_type idx) const {
    return at(idx);
  }

  void remove_prefix(size_type n) {
    DCHECK(length_ >= n);
    start_ += n;
    length_ -= n;
  }

  void remove_suffix(size_type n) {
    DCHECK(length_ >= n);
    length_ -= n;
  }

  int compare(const ByteStringPiece& x) const {
    int r = memcmp(start_, x.start_, std::min(length_, x.length_));
    if (r == 0) {
      if (length_ < x.length_) r = -1;
      else if (length_ > x.length_) r = +1;
    }
    return r;
  }

  const value_type* data() const {
    return start_;
  }

  const value_type* c_str() const {
    return start_;
  }

  // template<typename TString,
  //   typename = decltype(std::declval<TString>().append(std::declval<const char>(), std::declval<size_type>()))>
  template<typename TString>
  void appendTo(TString &aStr) {
    aStr.append(data(), size());
  }

  std::string copyToStdString() const {
    return std::string(data(), size());
  }


  // Does "this" start with "x"
  bool starts_with(const ByteStringPiece& x) const {
    return ((length_ >= x.length_) &&
            (memcmp(start_, x.start_, x.length_) == 0));
  }

  // Does "this" end with "x"
  bool ends_with(const ByteStringPiece& x) const {
    return ((length_ >= x.length_) &&
            (memcmp(start_ + (length_-x.length_), x.start_, x.length_) == 0));
  }

  using pointer = const value_type*;
  using const_pointer = const value_type*;
  using reference = const value_type&;
  using const_reference = const value_type&;
  using difference_type = std::ptrdiff_t;
  static const size_type npos = static_cast<size_type>(-1);
  using const_iterator = const value_type*;
  using iterator = const value_type*;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using reverse_iterator = std::reverse_iterator<iterator>;

  iterator begin() const {
    return start_;
  }

  iterator end() const {
    return start_ + length_;
  }

  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(start_ + length_);
  }

  const_reverse_iterator rend() const {
    return const_reverse_iterator(start_);
  }

  size_type max_size() const {
    return length_;
  }

  size_type capacity() const {
    return length_;
  }

  size_type copy(char *buff, size_type n, size_type offset = 0) const;
  size_type find(const ByteStringPiece&, size_type offset = 0) const;
  size_type find(value_type c, size_type offset = 0) const;
  size_type rfind(const ByteStringPiece&, size_type offset = npos) const;
  size_type rfind(value_type c, size_type offset = npos) const;

  ByteStringPiece substr(size_type offset, size_type n = npos) const;

  static bool _equal(const ByteStringPiece&, const ByteStringPiece&);
};

inline bool operator==(const ByteStringPiece& x, const ByteStringPiece& y) {
  return ByteStringPiece::_equal(x, y);
}

inline bool operator!=(const ByteStringPiece& x, const ByteStringPiece& y) {
  return !(x == y);
}

inline bool operator<(const ByteStringPiece& x, const ByteStringPiece& y) {
  const int r = memcmp(x.data(), y.data(),
                       std::min(x.size(), y.size()));
  return ((r < 0) || ((r == 0) && (x.size() < y.size())));
}

inline bool operator>(const ByteStringPiece& x, const ByteStringPiece& y) {
  return y < x;
}

inline bool operator<=(const ByteStringPiece& x, const ByteStringPiece& y) {
  return !(x > y);
}

inline bool operator>=(const ByteStringPiece& x, const ByteStringPiece& y) {
  return !(x < y);
}

}} // score::io


std::ostream& operator<<(std::ostream&, const score::io::ByteStringPiece&);
