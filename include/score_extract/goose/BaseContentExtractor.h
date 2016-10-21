#pragma once
#include "score_html/Node.h"
#include "score_extract/goose/BoostChecker.h"
#include "score_extract/goose/TextCleaner.h"
#include "score_extract/goose/TextNodeCollector.h"
#include "score_extract/goose/GooseOptions.h"
#include "score_extract/goose/NodeScorer.h"
#include "score_extract/goose/util.h"
#include "score_nlp/stemming/Utf8Stemmer.h"
#include <string>
#include <sstream>
#include <vector>

using namespace std;

namespace score { namespace extract { namespace goose {

using nlp::stemming::Utf8Stemmer;

template<typename TNodeScorer, typename TTextCleaner,
  typename TTextCollector, typename TStopwordCounter,
  typename TBoostChecker>
class BaseContentExtractor {
 public:
  std::string extract(const GooseOptions &opts,
      TStopwordCounter &stopwords, const html::Node &rootNode) {

    TBoostChecker boostChecker(opts, stopwords);
    TTextCollector collector(stopwords);
    TNodeScorer scorer(stopwords, collector, boostChecker, rootNode);
    TTextCleaner cleaner;
    scorer.process();
    auto topNode = scorer.getTopNode();
    int topNodeScore = scorer.getTopNodeScore();
    std::ostringstream contentOss;
    double thresholdScore = ((double) topNodeScore) * 0.08;
    size_t i = 0;
    for (auto node: topNode.children()) {
      i++;
      auto nodeText = cleaner.getText(node);
      if (node.isElement() && !node.hasTag(html::Tag::P)) {
        if (hasHighLinkDensity(node, nodeText)) {
          continue;
        }
        if (!node.hasTag(html::Tag::TD)) {
          auto nodeScore = (double) scorer.getNodeScore(node);
          if (nodeScore < thresholdScore) {
            continue;
          }
        }
      }
      if (stopwords.countStopwords(nodeText) < 3) {
        continue;
      }
      contentOss << nodeText;
    }
    return contentOss.str();
  }
};

}}} // score::extract::goose
