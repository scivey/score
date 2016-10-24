#pragma once
#include "score/nlp/stopwords/StopwordFilter.h"
#include "score/nlp/stemming/Utf8Stemmer.h"
#include "score/nlp/Language.h"
#include <string>
#include <memory>

namespace score { namespace extract { namespace goose {

class StupidStopwordCounter {
 protected:
  nlp::Language language_;
  nlp::stopwords::StopwordFilter stopwordFilter_;
  std::shared_ptr<nlp::stemming::Utf8Stemmer> stemmer_;
 public:
  StupidStopwordCounter(
    nlp::Language lang,
    std::shared_ptr<nlp::stemming::Utf8Stemmer>
  );
  size_t countStopwords(const std::string&);
};

}}} // score::extract::goose
