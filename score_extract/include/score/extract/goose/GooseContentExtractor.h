#pragma once
#include <string>
#include "score/html/HTMLDoc.h"
#include "score/nlp/Language.h"
#include "score/nlp/stemming/StemmerManager.h"
#include "score/extract/goose/GooseOptions.h"

namespace score { namespace extract { namespace goose {

class GooseContentExtractor {
  GooseOptions options_;
  nlp::stemming::StemmerManager stemmers_;
 public:
  GooseContentExtractor();
  GooseContentExtractor(GooseOptions);
  std::string extract(const html::HTMLDoc &dom, nlp::Language);
  std::string extract(const std::string &htmlStr, nlp::Language);
};

}}} // score::extract::goose
