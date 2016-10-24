#pragma once
#include <set>
#include <memory>
#include <unicode/brkiter.h>
#include <unicode/utext.h>
#include "score/nlp/Language.h"
#include "score/nlp/UTF8UTextRef.h"

namespace score { namespace nlp { namespace tokenize {

class ICUWordBreakView {
 protected:
  Language language_;
  std::unique_ptr<icu_52::BreakIterator> breakIter_;
  bool atEnd_ {false};
  UTF8UTextRef textRef_;

  ICUWordBreakView(Language lang, std::unique_ptr<icu_52::BreakIterator> breakIter);
  int32_t getNext();

 public:
  bool valid() const;
  bool hasText() const;
  static ICUWordBreakView create(Language lang);

  void setText(UTF8UTextRef &&textRef);
  void setText(const char*, size_t);

  template<typename TStr>
  void setText(const TStr &aString) {
    setText(aString.c_str(), aString.size());
  }

  void reset();
  class Iterator {
   protected:
    ICUWordBreakView *parent_ {nullptr};
    int32_t currentIndex_ {0};
   public:
    Iterator(ICUWordBreakView *parent);
    Iterator(ICUWordBreakView *parent, int32_t current);
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