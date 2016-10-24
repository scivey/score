#pragma once
#include <random>
#include "score/randomness/RandomEngine.h"

namespace score { namespace randomness {

template<template<class> class TTmpl, typename TElem>
class RandomDistribution {
 public:
  using distribution_type = TTmpl<TElem>;
  using value_type = TElem;
 protected:
  distribution_type dist_;
  RandomEngine<std::mt19937> engine_;
 public:
  RandomDistribution(value_type minVal, value_type maxVal, size_t seed)
    : dist_(minVal, maxVal), engine_(seed){}
  RandomDistribution(value_type minVal, value_type maxVal)
    : dist_(minVal, maxVal) {}
  static RandomDistribution createMaxInterval(size_t seed) {
    TElem nothing = 0;
    TElem everything = ~nothing;
    return RandomDistribution(nothing, everything, seed);
  }
  static RandomDistribution createMaxInterval() {
    TElem nothing = 0;
    TElem everything = ~nothing;
    return RandomDistribution(nothing, everything);
  }
  value_type get() {
    return dist_(engine_.get());
  }
  value_type getConstrained(value_type minV, value_type maxV) {
    auto interval = maxV - minV;
    return (get() % interval) + minV;
  }
};

}} // score::randomness