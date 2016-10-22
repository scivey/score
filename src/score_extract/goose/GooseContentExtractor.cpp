#include "score_extract/goose/GooseContentExtractor.h"
#include "score_extract/goose/BaseContentExtractor.h"
#include "score_extract/goose/BoostChecker.h"
#include "score_extract/goose/NodeScorer.h"
#include "score_extract/goose/StupidStopwordCounter.h"
#include "score_extract/goose/TextCleaner.h"
#include "score_extract/goose/TextNodeCollector.h"
#include "score_extract/goose/util.h"

using score::nlp::Language;
using namespace score::html;
using namespace std;

namespace score { namespace extract { namespace goose {

GooseContentExtractor::GooseContentExtractor(){}

GooseContentExtractor::GooseContentExtractor(GooseOptions options)
  : options_(options) {}

string GooseContentExtractor::extract(const HTMLDoc &dom, Language lang) {

  using collector_type = TextNodeCollector<
    TextCleaner, StupidStopwordCounter
  >;
  using booster_type = BoostChecker<
    TextCleaner, StupidStopwordCounter
  >;
  using scorer_type = NodeScorer<
    StupidStopwordCounter, collector_type,
    TextCleaner, booster_type
  >;

  StupidStopwordCounter counter(lang, stemmers_.getShared(lang));
  BaseContentExtractor<
    scorer_type, TextCleaner, collector_type,
    StupidStopwordCounter, booster_type
  > baseExtractor;
  return baseExtractor.extract(options_, counter, dom.root());
}

string GooseContentExtractor::extract(const string &htmlStr, Language lang) {
  auto dom = HTMLDoc::create(htmlStr);
  return extract(dom, lang);
}

}}} // score::extract::goose
