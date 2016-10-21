#pragma once
#include "score_html/Node.h"
#include "score_extract/goose/util.h"
#include <vector>
#include <set>

namespace score { namespace extract { namespace goose {

template<typename TCleaner, typename TStopwordCounter>
class TextNodeCollector {
 protected:
  std::set<html::Tag> topTags_ {
    html::Tag::P, html::Tag::PRE, html::Tag::TD
  };
  TCleaner cleaner_;
  TStopwordCounter &counter_;

 public:
  TextNodeCollector(TStopwordCounter &counter)
    : counter_(counter) {}

  std::vector<html::Node> collect(const html::Node &target) {
    std::vector<html::Node> withText;
    target.dfs([this, &withText](const html::Node &node) {
      auto tag = node.getTag();
      if (tag != html::Tag::P && tag != html::Tag::PRE && tag != html::Tag::TD) {
        return;
      }
      auto nodeText = cleaner_.getText(node);
      bool highLinks = hasHighLinkDensity(node, nodeText);
      size_t counts = counter_.countStopwords(nodeText);
      if (!highLinks && counts > 2) {
        withText.push_back(node);
      }
    });
    return withText;
  }

};

}}} // score::extract::goose
