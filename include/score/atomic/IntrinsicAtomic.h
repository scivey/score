#pragma once
#include <atomic>

namespace score { namespace atomic {

template<typename T>
class IntrinsicAtomic {
 public:
  using value_type = T;
  static const std::memory_order kDefaultMemoryOrder
    = std::memory_order::memory_order_seq_cst;

 protected:
  value_type data_ {0};
  static int intrinsicOrderOfStdOrder(std::memory_order order) {
    switch(order) {
      case std::memory_order::memory_order_relaxed:
        return __ATOMIC_RELAXED;
      case std::memory_order::memory_order_consume:
        return __ATOMIC_CONSUME;
      case std::memory_order::memory_order_acquire:
        return __ATOMIC_ACQUIRE;
      case std::memory_order::memory_order_release:
        return __ATOMIC_RELEASE;
      case std::memory_order::memory_order_acq_rel:
        return __ATOMIC_ACQ_REL;
      case std::memory_order::memory_order_seq_cst:
        return __ATOMIC_SEQ_CST;
      default:
        return __ATOMIC_SEQ_CST;
    }
  }

 public:
  IntrinsicAtomic(){}
  IntrinsicAtomic(value_type val): data_(val){}
  value_type load(std::memory_order order = kDefaultMemoryOrder) {
    return __atomic_load_n(&data_, intrinsicOrderOfStdOrder(order));
  }
  void store(value_type val, std::memory_order order = kDefaultMemoryOrder) {
    __atomic_store_n(&data_, val, intrinsicOrderOfStdOrder(order));
  }
 protected:
  bool compare_exchange(value_type *expected,
      value_type desired, bool weak,
      std::memory_order successOrder,
      std::memory_order failureOrder) {
    return __atomic_compare_exchange_n(
      &data_, expected, desired, weak,
      intrinsicOrderOfStdOrder(successOrder),
      intrinsicOrderOfStdOrder(failureOrder)
    );
  }
 public:
  bool compare_exchange_strong(value_type *expected, value_type desired, std::memory_order order = kDefaultMemoryOrder) {
    bool weak = false;
    return compare_exchange(expected, desired, weak, order, order);
  }
  bool compare_exchange_weak(value_type *expected, value_type desired, std::memory_order order = kDefaultMemoryOrder) {
    bool weak = true;
    return compare_exchange(expected, desired, weak, order, order);
  }
};

}} // score::atomic
