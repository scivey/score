#pragma once

namespace asioz {

template<size_t NIdent>
class Counter {
 protected:
  size_t counter_ {0};
 public:
  static const size_t ID = NIdent;

  size_t get() {
    auto result = counter_;
    counter_++;
    return result;
  }

  void reset() {
    counter_ = 0;
  }
};

} // asioz
