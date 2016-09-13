#pragma once
#include <set>
#include <memory>
#include <unicode/brkiter.h>
#include "score_nlp/Language.h"


namespace score { namespace nlp { namespace tokenize {

class ICUWordBreakView {
 public:
 protected:
  Language language_;
  std::unique_ptr<icu_52::BreakIterator> breakIter_;
  ICUWordBreakView(Language lang, std::unique_ptr<icu_52::BreakIterator> breakIter);
  icu_52::UnicodeString *target_ {nullptr};
  bool atEnd_ {false};

  int32_t getNext();

 public:
  bool valid() const;
  bool hasText() const;
  static ICUWordBreakView create(Language lang);
  void setText(icu_52::UnicodeString &target);
  void setText(icu_52::UnicodeString *target);
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