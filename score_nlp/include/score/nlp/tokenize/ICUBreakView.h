#pragma once
#include <set>
#include <memory>
#include <unicode/brkiter.h>
#include <unicode/utext.h>
#include "score/macros.h"
#include "score/nlp/Language.h"
#include "score/nlp/UTF8UTextRef.h"

namespace score { namespace nlp { namespace tokenize {

class ICUBreakView {
 protected:
  Language language_;
  std::unique_ptr<icu::BreakIterator> breakIter_;
  bool atEnd_ {false};
  UTF8UTextRef textRef_;

  ICUBreakView(Language lang, std::unique_ptr<icu::BreakIterator> breakIter);
  int32_t getNext();

 public:
  bool valid() const;
  bool hasText() const;
  static ICUBreakView create(Language lang);

  const UTF8UTextRef& getUTextRef() const {
    return textRef_;
  }

  io::ByteStringPiece getByteStringPiece() const {
    SDCHECK(valid());
    return textRef_.toByteStringPiece();
  }


  void setText(UTF8UTextRef &&textRef);
  void setText(const char*, size_t);

  template<typename TStr>
  void setText(const TStr &aString) {
    setText(aString.c_str(), aString.size());
  }

  void reset();
  class Iterator {
   protected:
    ICUBreakView *parent_ {nullptr};
    int32_t currentIndex_ {0};
   public:
    using value_type = int32_t;
    Iterator(ICUBreakView *parent);
    Iterator(ICUBreakView *parent, int32_t current);
    bool operator!=(const Iterator &other) const;
    Iterator& operator++();
    Iterator operator++(int);
    int32_t operator*();
  };

  friend class Iterator;
  using iterator = Iterator;

  Iterator begin();
  Iterator end();

};



}}} // score::nlp::tokenize