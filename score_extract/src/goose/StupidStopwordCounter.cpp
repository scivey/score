#include "score/extract/goose/StupidStopwordCounter.h"
#include "score/nlp/stopwords/StopwordFilter.h"
#include "score/nlp/tokenize/ICUTokenView.h"
#include "score/nlp/tokenize/predicates.h"
#include "score/nlp/Language.h"
#include "score/iter/IFilter.h"
#include <string>

using namespace std;
namespace score { namespace extract { namespace goose {

using nlp::Language;
using nlp::stemming::Utf8Stemmer;
using nlp::tokenize::ICUTokenView;

StupidStopwordCounter::StupidStopwordCounter(Language lang,
    shared_ptr<Utf8Stemmer> stemmer)
  : language_(lang), stopwordFilter_(lang), stemmer_(stemmer) {}

size_t StupidStopwordCounter::countStopwords(const string &text) {
  auto tokenView = ICUTokenView::create(language_);
  tokenView.setText(text);
  string currentToken = "";
  const char *beginning = text.c_str();
  size_t count = 0;
  for (auto toke: tokenView) {
    if (nlp::tokenize::isWordToken(toke)) {
      currentToken = toke.copyToStdString();
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
