#pragma once
#include <string>
#include "score_nlp/Language.h"

struct sb_stemmer;

namespace score { namespace nlp { namespace stemming {

class SbStemmerWrapper {
 protected:
  std::string countryCode_;
  struct sb_stemmer *stemmer_ {nullptr};
  SbStemmerWrapper(std::string countryCode, struct sb_stemmer*);
  SbStemmerWrapper(const SbStemmerWrapper &other) = delete;
  SbStemmerWrapper& operator=(const SbStemmerWrapper &other) = delete;
 public:
  SbStemmerWrapper& operator=(SbStemmerWrapper &&other);
  SbStemmerWrapper(SbStemmerWrapper &&other);
  static SbStemmerWrapper create(std::string countryCode);
  size_t getStemPos(const char *toStem, size_t length);
  size_t getStemPos(const std::string&);
  ~SbStemmerWrapper();
};

}}} // score::nlp::stemming
