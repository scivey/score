#include <string>
#include <set>
#include "score_nlp/stopwords/MultiLanguageStopwordFilter.h"
#include "score_nlp/Language.h"
#include "score_nlp/stopwords/english_stopwords.h"
#include "score_nlp/stopwords/french_stopwords.h"
#include "score_nlp/stopwords/german_stopwords.h"
#include "score_nlp/stopwords/italian_stopwords.h"
#include "score_nlp/stopwords/russian_stopwords.h"
#include "score_nlp/stopwords/spanish_stopwords.h"

using namespace std;

namespace score { namespace nlp { namespace stopwords {

bool MultiLanguageStopwordFilter::isStopword(const string &word, Language lang) {
  if (word.size() <= 2) {
    return true;
  }
  switch (lang) {
    case Language::DE : return isGermanStopword(word);
    case Language::EN : return isEnglishStopword(word);
    case Language::ES : return isSpanishStopword(word);
    case Language::FR : return isFrenchStopword(word);
    case Language::IT : return isItalianStopword(word);
    case Language::RU : return isRussianStopword(word);
    default           : return false;
  }
}

}}} // score::nlp::stopwords
