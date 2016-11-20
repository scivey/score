#pragma once
#include <string>
#include "score/nlp/tokenize/ICUBreakView.h"
#include "score/nlp/Language.h"
#include "score/iter/IterTrailer.h"
#include "score/io/ByteStringPiece.h"



namespace score { namespace nlp { namespace tokenize {

class ICUTokenOffsetView {
 protected:
  Language language_;
  ICUBreakView breakView_;

  ICUTokenOffsetView(Language lang, ICUBreakView&& breakView)
    : language_(lang), breakView_(std::forward<ICUBreakView>(breakView)){}
 public:

  static ICUTokenOffsetView create(Language lang) {
    return ICUTokenOffsetView(lang, ICUBreakView::create(lang));
  }

  using iterator = iter::TrailingIterator<typename ICUBreakView::iterator>;

  iterator begin() {
    return iter::IterTrailer<ICUBreakView>(breakView_).begin();
  }

  iterator end() {
    return iter::IterTrailer<ICUBreakView>(breakView_).end();
  }

  template<typename ...Types>
  void setText(Types&&... args) {
    breakView_.setText(std::forward<Types>(args)...);
  }

  const UTF8UTextRef& getUTextRef() const {
    return breakView_.getUTextRef();
  }

  io::ByteStringPiece getByteStringPiece() const {
    return breakView_.getByteStringPiece();
  }

};

}}} // score::nlp::tokenize
