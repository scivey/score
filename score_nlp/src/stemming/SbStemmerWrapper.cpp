#include "score_nlp/stemming/SbStemmerWrapper.h"
#include "score_nlp/Language.h"
#include "score_nlp/vendored/libstemmer/libstemmer.h"
#include <string>
#include <cstring>

using namespace std;

namespace score { namespace nlp { namespace stemming {

SbStemmerWrapper::SbStemmerWrapper(string countryCode, struct sb_stemmer *stemmer)
  : countryCode_(countryCode), stemmer_(stemmer) {}

SbStemmerWrapper::SbStemmerWrapper(SbStemmerWrapper &&other)
  : countryCode_(other.countryCode_), stemmer_(other.stemmer_) {
  other.stemmer_ = nullptr;
}

SbStemmerWrapper& SbStemmerWrapper::operator=(SbStemmerWrapper &&other) {
  stemmer_ = other.stemmer_;
  countryCode_ = other.countryCode_;
  other.stemmer_ = nullptr;
  return *this;
}

size_t SbStemmerWrapper::getStemPos(const char *toStem, size_t length) {
  sb_stemmer_stem(stemmer_, (const sb_symbol*) toStem, length);
  return sb_stemmer_length(stemmer_);
}

size_t SbStemmerWrapper::getStemPos(const string &text) {
  return getStemPos(text.c_str(), text.size());
}

SbStemmerWrapper::~SbStemmerWrapper() {
  if (stemmer_) {
    sb_stemmer_delete(stemmer_);
    stemmer_ = nullptr;
  }
}

SbStemmerWrapper SbStemmerWrapper::create(string countryCode) {
  const char *cCode = countryCode.c_str();
  if (strcmp(cCode, "UNKNOWN")) {
    cCode = "en";
  }
  auto stemmer = sb_stemmer_new(cCode, "UTF_8");
  return SbStemmerWrapper(countryCode, stemmer);
}

}}} // score::nlp::stemming

