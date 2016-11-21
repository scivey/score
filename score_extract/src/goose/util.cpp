#include "score/extract/goose/util.h"
#include "score/nlp/tokenize/ICUTokenView.h"
#include "score/nlp/tokenize/predicates.h"

using namespace std;
using namespace score::html;
using score::nlp::Language;
using score::nlp::tokenize::ICUTokenView;
using score::nlp::tokenize::isWordToken;

namespace score { namespace extract { namespace goose {

size_t getTokenCount(const string &text) {
  auto tokenView = ICUTokenView::create(Language::EN);
  tokenView.setText(text);
  size_t tokenCount = 0;
  for (auto toke : tokenView) {
    if (isWordToken(toke)) {
      tokenCount++;
    }
  }
  return tokenCount;
}

bool hasHighLinkDensity(const Node &node, const string &nodeText) {
  size_t linkCount = 0;
  size_t linkTokenCount = 0;
  node.dfs([&linkCount, &linkTokenCount](const Node &elem) {
    if (!elem.hasTag(Tag::A)) {
      return;
    }
    linkCount++;
    linkTokenCount += getTokenCount(elem.getText());
  });
  if (linkCount == 0) {
    return false;
  }
  size_t nodeTokens = getTokenCount(nodeText);
  auto linkDivisor = ((double) linkTokenCount) / ((double) nodeTokens);
  auto score = linkDivisor * ((double) linkCount);
  return score > 1.0;
}

bool hasHighLinkDensity(const Node &node) {
  return hasHighLinkDensity(node, node.getText());
}

}}} // score::extract::goose
