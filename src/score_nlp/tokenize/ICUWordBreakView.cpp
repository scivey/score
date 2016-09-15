#include "score_nlp/tokenize/ICUWordBreakView.h"
#include "score/macros.h"
#include <unicode/locid.h>
#include <unicode/brkiter.h>

using namespace std;

namespace score { namespace nlp { namespace tokenize {

ICUWordBreakView::ICUWordBreakView(Language lang,
    unique_ptr<icu_52::BreakIterator> breakIter)
  : language_(lang), breakIter_(std::move(breakIter)) {}

bool ICUWordBreakView::valid() const {
  return !!breakIter_;
}

bool ICUWordBreakView::hasText() const {
  return !!textRef_;
}

ICUWordBreakView::Iterator ICUWordBreakView::begin() {
  if (!valid()) {
    return Iterator(this);
  }
  return Iterator(this, breakIter_->first());
}

ICUWordBreakView::Iterator ICUWordBreakView::end() {
  return Iterator(this);
}

// void ICUWordBreakView::setText(UnicodeString *target) {
//   SDCHECK(valid());
//   target_ = target;
//   atEnd_ = false;
//   breakIter_->setText(*target_);
// }

int32_t ICUWordBreakView::getNext() {
  SDCHECK(valid());
  auto nxt = breakIter_->next();
  if (nxt == icu_52::BreakIterator::DONE) {
    atEnd_ = true;
  }
  return nxt;
}

void ICUWordBreakView::reset() {
  if (valid() && textRef_) {
    UErrorCode status = U_ZERO_ERROR;
    breakIter_->setText(textRef_.getUText(), status);
    SCHECK(U_SUCCESS(status));
  }
}

void ICUWordBreakView::setText(UTF8UTextRef &&textRef) {
  SDCHECK(valid());
  textRef_ = std::move(textRef);
  reset();
}

void ICUWordBreakView::setText(const char *buff, size_t buffLen) {
  SDCHECK(valid());
  setText(UTF8UTextRef::fromUTF8(buff, buffLen));
}

ICUWordBreakView ICUWordBreakView::create(Language lang) {
  auto langCode = getLanguageCode(lang);
  UErrorCode icuStatus = U_ZERO_ERROR;
  std::unique_ptr<icu_52::BreakIterator> breakIter {
    BreakIterator::createWordInstance(
      icu_52::Locale(langCode), icuStatus
    )
  };
  return ICUWordBreakView(lang, std::move(breakIter));
}


ICUWordBreakView::Iterator::Iterator(ICUWordBreakView *parent)
  : parent_(parent), currentIndex_(icu_52::BreakIterator::DONE) {}

ICUWordBreakView::Iterator::Iterator(ICUWordBreakView *parent, int32_t current)
  : parent_(parent), currentIndex_(current) {}

bool ICUWordBreakView::Iterator::operator!=(const Iterator &other) const {
  return currentIndex_ != other.currentIndex_;
}

ICUWordBreakView::Iterator& ICUWordBreakView::Iterator::operator++() {
  currentIndex_ = parent_->getNext();
  return *this;
}

ICUWordBreakView::Iterator ICUWordBreakView::Iterator::operator++(int) {
  ICUWordBreakView::Iterator result = *this;
  ++*this;
  return result;
}

int32_t ICUWordBreakView::Iterator::operator*() {
  return currentIndex_;
}

}}} // score::nlp::tokenize