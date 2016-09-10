#pragma once
#include "score_nlp/stemming/Utf8Stemmer.h"
#include "score_nlp/Language.h"
#include <memory>
#include <string>
#include <map>

namespace score { namespace nlp { namespace stemming {

class StemmerManager {
 protected:
  std::map<Language, std::shared_ptr<Utf8Stemmer>> stemmers_;
  std::shared_ptr<Utf8Stemmer>& getOrCreate(Language lang);
 public:
  std::shared_ptr<Utf8Stemmer> getShared(Language lang);
  Utf8Stemmer* get(Language lang);
};

}}} // score::nlp::stemming

