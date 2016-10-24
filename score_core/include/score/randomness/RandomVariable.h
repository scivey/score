#pragma once
#include "score/randomness/RandomDistribution.h"

namespace score { namespace randomness {

template<typename TDistribution>
class RandomVariable {
 public:
  using distribution_type = TDistribution;
  using value_type = typename TDistribution::value_type;
 protected:
  distribution_type distrib_;
  value_type value_;
 public:
  RandomVariable(value_type param1, value_type param2, size_t seed)
    : distrib_(param1, param2, seed) {
    warble();
  }
  RandomVariable(value_type param1, value_type param2)
    : distrib_(param1, param2) {
    warble();
  }
  value_type get() const {
    return value_;
  }
  void warble() {
    value_ = distrib_.get();
  }
};

}} // score::randomness