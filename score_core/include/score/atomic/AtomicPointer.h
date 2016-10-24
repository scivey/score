#pragma once

#include <atomic>

namespace score { namespace atomic {


template<typename T>
class AtomicPointer {
 protected:
  std::atomic<uintptr_t> ptrVal_ {0};
 public:
  AtomicPointer(T *ptr) {
    store(ptr);
  }
  AtomicPointer(){}
  AtomicPointer(const AtomicPointer& other) {
    ptrVal_.store(other.ptrVal_.load());
  }
  AtomicPointer(AtomicPointer&& other) {
    uintptr_t val = other.ptrVal_.load();
    other.ptrVal_.store(0);
    ptrVal_.store(val);
  }
  AtomicPointer& operator=(AtomicPointer&& other) {
    uintptr_t otherVal = other.ptrVal_.load();
    uintptr_t selfVal = ptrVal_.load();
    ptrVal_.store(otherVal);
    other.ptrVal_.store(selfVal);
    return *this;
  }
  bool good() const {
    return get() != nullptr;
  }
  operator bool() const {
    return good();
  }
  AtomicPointer& operator=(const AtomicPointer& other) {
    ptrVal_.store(other.ptrVal_.load());
    return *this;
  }
  T* get() const {
    return (T*) ptrVal_.load();
  }
  void store(T *ptr) {
    ptrVal_.store((uintptr_t) ptr);
  }
  T* operator->() {
    return get();
  }
  AtomicPointer& operator=(const T* ptr) {
    store(ptr);
    return *this;
  }
};


}} // score::atomic
