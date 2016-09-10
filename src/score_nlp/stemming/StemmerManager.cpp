#include "score_nlp/stemming/Utf8Stemmer.h"
#include "score_nlp/stemming/StemmerManager.h"
#include "score_nlp/Language.h"
#include <memory>
#include <string>
#include <map>

using namespace std;

namespace score { namespace nlp { namespace stemming {

shared_ptr<Utf8Stemmer>& StemmerManager::getOrCreate(Language lang) {
  if (stemmers_.count(lang) == 0) {
    stemmers_.insert(std::make_pair(
      lang, std::make_shared<Utf8Stemmer>(lang)
    ));
  }
  return stemmers_[lang];
}

shared_ptr<Utf8Stemmer> StemmerManager::getShared(Language lang) {
  return getOrCreate(lang);
}

Utf8Stemmer* StemmerManager::get(Language lang) {
  return getOrCreate(lang).get();
}

}}} // score::nlp::stemming

