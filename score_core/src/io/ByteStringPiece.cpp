#include "score/io/ByteStringPiece.h"
#include <algorithm>
#include <string>
#include <type_traits>

namespace score { namespace io {

using length_type = ByteStringPiece::length_type;
using const_pointer = ByteStringPiece::const_pointer;

const ByteStringPiece::length_type ByteStringPiece::npos;  // initialized in stringpiece.h

ByteStringPiece::length_type ByteStringPiece::copy(char* buf, length_type n,
                                         length_type pos) const {
  length_type ret = std::min(length_ - pos, n);
  memcpy(buf, start_ + pos, ret);
  return ret;
}

ByteStringPiece ByteStringPiece::substr(length_type pos, length_type n) const {
  if (pos > length_) pos = length_;
  if (n > length_ - pos) n = length_ - pos;
  return ByteStringPiece(start_ + pos, n);
}

ByteStringPiece::length_type ByteStringPiece::find(const ByteStringPiece& s,
                                         length_type pos) const {
  if (pos > length_) return npos;
  const_pointer result = std::search(start_ + pos, start_ + length_,
                                     s.start_, s.start_ + s.length_);
  length_type xpos = result - start_;
  return xpos + s.length_ <= length_ ? xpos : npos;
}

ByteStringPiece::length_type ByteStringPiece::find(char c, length_type pos) const {
  if (length_ <= 0 || pos >= length_) return npos;
  const_pointer result = std::find(start_ + pos, start_ + length_, c);
  return result != start_ + length_ ? result - start_ : npos;
}

ByteStringPiece::length_type ByteStringPiece::rfind(const ByteStringPiece& s,
                                          length_type pos) const {
  if (length_ < s.length_) return npos;
  if (s.length_ == 0) return std::min(length_, pos);
  const_pointer last = start_ + std::min(length_ - s.length_, pos) + s.length_;
  const_pointer result = std::find_end(start_, last, s.start_, s.start_ + s.length_);
  return result != last ? result - start_ : npos;
}

ByteStringPiece::length_type ByteStringPiece::rfind(char c, length_type pos) const {
  if (length_ <= 0) return npos;
  for (length_type i = std::min(pos + 1, length_); i != 0;) {
    if (start_[--i] == c) return i;
  }
  return npos;
}


}} // score::io

std::ostream& operator<<(std::ostream& o, const score::io::ByteStringPiece& p) {
  o.write(p.data(), p.size());
  return o;
}
