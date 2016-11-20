#include "score/extract/goose/StupidStopwordCounter.h"
#include "score/nlp/stopwords/StopwordFilter.h"
#include "score/nlp/tokenize/TokenView.h"
#include "score/nlp/tokenize/ICUTokenOffsetView.h"
#include "score/nlp/Language.h"
#include <string>

using namespace std;
namespace score { namespace extract { namespace goose {

using nlp::Language;
using nlp::stemming::Utf8Stemmer;
using nlp::tokenize::TokenView;
using nlp::tokenize::ICUTokenOffsetView;

StupidStopwordCounter::StupidStopwordCounter(Language lang,
    shared_ptr<Utf8Stemmer> stemmer)
  : language_(lang), stopwordFilter_(lang), stemmer_(stemmer) {}

size_t StupidStopwordCounter::countStopwords(const string &text) {
  auto breakView = ICUTokenOffsetView::create(language_);
  breakView.setText(text);
  string currentToken = "";
  const char *beginning = text.c_str();
  size_t count = 0;
  for (auto toke: breakView) {
    const char *tokenStart = beginning + toke.first;
    if (toke.second <= toke.first) {
      break;
    }
    size_t diff = toke.second - toke.first;
    currentToken.clear();
    const char *it = tokenStart;
    for (size_t i = 0; i < diff; i++) {
      currentToken.push_back(*it);
      it++;
    }
    if (stopwordFilter_.isStopword(currentToken)) {
      count++;
    } else {
      currentToken.erase(stemmer_->getStemPos(currentToken));
      if (stopwordFilter_.isStopword(currentToken)) {
        count++;
      }
    }

  }
  return count;
}

}}} // score::extract::goose
