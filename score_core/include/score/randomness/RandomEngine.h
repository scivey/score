#pragma once
#include <random>

namespace score { namespace randomness {

template<typename TEngine>
class RandomEngine {
 public:
  using engine_type = TEngine;
 protected:
  TEngine engine_;
 public:
  RandomEngine(size_t seed): engine_(seed){}
  RandomEngine() {
    std::random_device rd;
    engine_.seed(rd());
  }
  engine_type& get() {
    return engine_;
  }
};

}} // score::randomness
