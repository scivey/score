#pragma once
#include <cstddef>

namespace score { namespace extract { namespace goose {

class GooseOptions {
 protected:
  size_t minBoostStopwords_ {5};
  size_t maxBoostDistance_ {3};
 public:
  size_t minBoostStopwords() const;
  void minBoostStopwords(size_t);
  size_t maxBoostDistance() const;
  void maxBoostDistance(size_t);
};

}}} // score::extract::goose
