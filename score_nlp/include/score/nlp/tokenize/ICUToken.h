#pragma once
#include <string>
#include "score/nlp/tokenize/ICUTokenOffsetView.h"
#include "score/nlp/Language.h"
#include "score/nlp/UTF8UTextRef.h"
#include "score/iter/IterTrailer.h"

namespace score { namespace nlp { namespace tokenize {


class ICUToken {
 public:
  using offset_pair_t = std::pair<size_t, size_t>;
 protected:
  io::ByteStringPiece parentText_;
  offset_pair_t offsets_;
 public:
  ICUToken(io::ByteStringPiece parentText, offset_pair_t offsets)
    : parentText_(parentText), offsets_(offsets) {}

  bool valid() const {
    return offsets_.first <= offsets_.second && parentText_.valid();
  }
  const offset_pair_t& offsets() const {
    return offsets_;
  }
  UTF8UTextRef toUTextRef() const {
    auto asBytes = toByteStringPiece();
    return UTF8UTextRef::fromUTF8(asBytes.c_str(), asBytes.size());
  }
  io::ByteStringPiece toByteStringPiece() const {
    SDCHECK(valid());
    const char *base = parentText_.data();
    base += offsets_.first;
    return io::ByteStringPiece {base, byteSize()};
  }
  std::string copyToStdString() {
    return toByteStringPiece().copyToStdString();
  }
  size_t byteSize() const {
    if (LIKELY(valid())) {
      return offsets_.second - offsets_.first;
    }
    return 0;
  }
};

}}} // score::nlp::tokenize
