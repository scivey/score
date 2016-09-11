#pragma once
#include <string>
#include "score_nlp/Language.h"

namespace score { namespace nlp { namespace stopwords {

class MultiLanguageStopwordFilter {
 public:
  bool isStopword(
    const std::string &stemmedWord,
    Language language
  );
};


}}} // score::nlp::stopwords
