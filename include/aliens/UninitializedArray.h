#pragma once
#include <bitset>
#include <array>
#include <glog/logging.h>

namespace aliens {

template<typename T, size_t NItems>
class UninitializedArray {
 public:
  using value_type = T;
  static const size_t N = NItems;
  static const size_t NBytes = NItems * sizeof(T);
 protected:
  std::bitset<NItems> initBitMap_ {0};
  unsigned char data_ [NBytes];

  T* privatePointerAt(size_t idx) {
    // doesn't check whether item is initialized.
    CHECK(idx <= NItems);
    auto base = (T*) ((unsigned char*) data_);
    return base + idx;
  }
 public:
  size_t size() {
    return initBitMap_.count();
  }
  T* pointerAt(size_t idx) {
    CHECK(isInitialized(idx));
    return privatePointerAt(idx);
  }
  T& refAt(size_t idx) {
    auto ptr = pointerAt(idx);
    return *ptr;
  }
  bool isInitialized(size_t idx) {
    return initBitMap_.test(idx);
  }
  template<typename ...Args>
  bool initialize(size_t idx, Args&& ...args) {
    if (isInitialized(idx)) {
      return false;
    }
    auto ptr = privatePointerAt(idx);
    new (ptr) T(std::forward<Args...>(args...));
    initBitMap_.set(idx);
    return true;
  }
};

} // aliens

