#include <string>
#include "score/nlp/stemming/Utf8Stemmer.h"
#include "score/nlp/Language.h"

using namespace std;

namespace score { namespace nlp { namespace stemming {

Utf8Stemmer::Utf8Stemmer(Language lang)
  : language_(lang),
    stemmer_(SbStemmerWrapper::create(stringOfLanguage(lang))) {}

size_t Utf8Stemmer::getStemPos(const char *toStem, size_t length) {
  return stemmer_.getStemPos(toStem, length);
}

size_t Utf8Stemmer::getStemPos(const string &text) {
  return stemmer_.getStemPos(text);
}

void Utf8Stemmer::stemInPlace(string &text) {
  auto idx = getStemPos(text);
  text.resize(idx);
}

std::string Utf8Stemmer::stem(const string &text) {
  string copied = text;
  stemInPlace(copied);
  return copied;
}

}}} // score::nlp::stemming

