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


class ICUTokenView {
 protected:
  Language language_;
  ICUTokenOffsetView offsetView_;

  ICUTokenView(Language lang, ICUTokenOffsetView&& offsetView)
    : language_(lang), offsetView_(std::forward<ICUTokenOffsetView>(offsetView)){}
 public:

  static ICUTokenView create(Language lang) {
    return ICUTokenView(lang, ICUTokenOffsetView::create(lang));
  }

  class Iterator {
   public:
    using value_type = ICUToken;
    using sub_iter_t = typename ICUTokenOffsetView::iterator;
   protected:
    io::ByteStringPiece parentText_;
    sub_iter_t subIter_;
   public:
    Iterator(io::ByteStringPiece parentText, sub_iter_t&& subIter)
      : parentText_(parentText),
        subIter_(std::forward<sub_iter_t>(subIter)){}

    ICUToken operator*() {
      return ICUToken(parentText_, *subIter_);
    }
    Iterator& operator++() {
      ++subIter_;
      return *this;
    }
    Iterator operator++(int) {
      Iterator other = *this;
      ++subIter_;
      return other;
    }
    bool operator!=(const Iterator& other) {
      return subIter_ != other.subIter_;
    }

  };

  using iterator = Iterator;

  iterator begin() {
    return Iterator {
      offsetView_.getByteStringPiece(),
      offsetView_.begin()
    };
  }

  iterator end () {
    return Iterator {
      offsetView_.getByteStringPiece(),
      offsetView_.end()
    };
  }

  template<typename ...Types>
  void setText(Types&&... args) {
    offsetView_.setText(std::forward<Types>(args)...);
  }

};

}}} // score::nlp::tokenize
