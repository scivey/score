#include <string>
#include "score/nlp/stopwords/StopwordFilter.h"
#include "score/nlp/Language.h"

using namespace std;

namespace score { namespace nlp { namespace stopwords {

StopwordFilter::StopwordFilter(Language lang): language_(lang){}

bool StopwordFilter::isStopword(const string &stemmedWord) {
  return underlyingFilter_.isStopword(stemmedWord, language_);
}

}}} // score::nlp::stopwords
