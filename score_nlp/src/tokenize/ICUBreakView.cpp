#include "score/nlp/tokenize/ICUBreakView.h"
#include "score/macros.h"
#include <unicode/locid.h>
#include <unicode/brkiter.h>

using namespace std;

namespace score { namespace nlp { namespace tokenize {

ICUBreakView::ICUBreakView(Language lang,
    unique_ptr<icu::BreakIterator> breakIter)
  : language_(lang), breakIter_(std::move(breakIter)) {}

bool ICUBreakView::valid() const {
  return !!breakIter_;
}

bool ICUBreakView::hasText() const {
  return !!textRef_;
}

ICUBreakView::Iterator ICUBreakView::begin() {
  if (!valid()) {
    return Iterator(this);
  }
  return Iterator(this, breakIter_->first());
}

ICUBreakView::Iterator ICUBreakView::end() {
  return Iterator(this);
}

// void ICUBreakView::setText(UnicodeString *target) {
//   SDCHECK(valid());
//   target_ = target;
//   atEnd_ = false;
//   breakIter_->setText(*target_);
// }

int32_t ICUBreakView::getNext() {
  SDCHECK(valid());
  auto nxt = breakIter_->next();
  if (nxt == icu::BreakIterator::DONE) {
    atEnd_ = true;
  }
  return nxt;
}

void ICUBreakView::reset() {
  if (valid() && textRef_) {
    UErrorCode status = U_ZERO_ERROR;
    breakIter_->setText(textRef_.getUText(), status);
    SCHECK(U_SUCCESS(status));
  }
}

void ICUBreakView::setText(UTF8UTextRef &&textRef) {
  SDCHECK(valid());
  textRef_ = std::move(textRef);
  reset();
}

void ICUBreakView::setText(const char *buff, size_t buffLen) {
  SDCHECK(valid());
  setText(UTF8UTextRef::fromUTF8(buff, buffLen));
}

ICUBreakView ICUBreakView::create(Language lang) {
  auto langCode = getLanguageCode(lang);
  UErrorCode icuStatus = U_ZERO_ERROR;
  std::unique_ptr<icu::BreakIterator> breakIter {
    BreakIterator::createWordInstance(
      icu::Locale(langCode), icuStatus
    )
  };
  return ICUBreakView(lang, std::move(breakIter));
}


ICUBreakView::Iterator::Iterator(ICUBreakView *parent)
  : parent_(parent), currentIndex_(icu::BreakIterator::DONE) {}

ICUBreakView::Iterator::Iterator(ICUBreakView *parent, int32_t current)
  : parent_(parent), currentIndex_(current) {}

bool ICUBreakView::Iterator::operator!=(const Iterator &other) const {
  return currentIndex_ != other.currentIndex_;
}

ICUBreakView::Iterator& ICUBreakView::Iterator::operator++() {
  currentIndex_ = parent_->getNext();
  return *this;
}

ICUBreakView::Iterator ICUBreakView::Iterator::operator++(int) {
  ICUBreakView::Iterator result = *this;
  ++*this;
  return result;
}

int32_t ICUBreakView::Iterator::operator*() {
  return currentIndex_;
}

}}} // score::nlp::tokenize