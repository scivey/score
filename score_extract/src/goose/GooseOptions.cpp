#include "score_extract/goose/GooseOptions.h"

namespace score { namespace extract { namespace goose {

size_t GooseOptions::minBoostStopwords() const {
  return minBoostStopwords_;
}

void GooseOptions::minBoostStopwords(size_t val) {
  minBoostStopwords_ = val;
}

size_t GooseOptions::maxBoostDistance() const {
  return maxBoostDistance_;
}

void GooseOptions::maxBoostDistance(size_t val) {
  maxBoostDistance_ = val;
}

}}} // score::extract::goose
