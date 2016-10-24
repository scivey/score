#pragma once
#include <string>
#include "score_nlp/stopwords/MultiLanguageStopwordFilter.h"
#include "score_nlp/Language.h"

namespace score { namespace nlp { namespace stopwords {

class StopwordFilter {
 protected:
  Language language_;
  MultiLanguageStopwordFilter underlyingFilter_;
 public:
  StopwordFilter(Language lang);
  bool isStopword(const std::string &stemmedWord);
};


}}} // score::nlp::stopwords
