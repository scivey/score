#pragma once
#include <map>
#include <set>
#include <vector>
#include <memory>
#include "score/html/Node.h"
#include "score/html/Tag.h"

namespace score { namespace extract { namespace goose {

template<typename TCleaner, typename TStopwordCounter>
class BoostChecker {
 protected:
  const GooseOptions &options_;
  TCleaner cleaner_;
  TStopwordCounter &stopwordCounter_;
 public:
  BoostChecker(const GooseOptions &opts, TStopwordCounter &counter)
    : options_(opts), stopwordCounter_(counter) {}

  bool shouldBoost(const html::Node &node) {
    bool isOk = false;
    size_t stepsAway = 0;
    node.walkSiblings(
      [this, &isOk, &stepsAway]
      (const html::Node &sibling, std::function<void()> escape) {
        if (stepsAway >= this->options_.maxBoostDistance()) {
          isOk = false;
          escape();
          return;
        }
        if (sibling.hasTag(html::Tag::P)) {
          auto siblingText = this->cleaner_.getText(sibling);
          auto siblingStops = this->stopwordCounter_.countStopwords(siblingText);
          if (siblingStops > this->options_.minBoostStopwords()) {
            isOk = true;
            escape();
            return;
          }
          stepsAway++;
        }
      }
    );
    return isOk;
  }
};

}}} // score::extract::goose
