#pragma once
#include <string>
#include "score/nlp/Language.h"
#include "score/nlp/stemming/SbStemmerWrapper.h"

namespace score { namespace nlp { namespace stemming {

class Utf8Stemmer {
protected:
  Language language_;
  SbStemmerWrapper stemmer_;
public:
  Utf8Stemmer(Language lang);
  size_t getStemPos(const char *toStem, size_t length);
  size_t getStemPos(const std::string&);
  void stemInPlace(std::string&);
  std::string stem(const std::string&);
};

}}} // score::nlp::stemming
