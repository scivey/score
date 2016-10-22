#pragma once
#include <string>
#include "score_html/HTMLDoc.h"
#include "score_nlp/Language.h"
#include "score_nlp/stemming/StemmerManager.h"
#include "score_extract/goose/GooseOptions.h"

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
