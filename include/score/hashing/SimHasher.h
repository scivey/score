#pragma once
#include <functional>
#include <bitset>
#include <array>

namespace score { namespace hashing {

template<typename TValue>
class SimHasher {
 public:
  static const size_t N = 64;
  using value_type = TValue;
  using hashed_type = uint64_t;
  using HashFunc = std::function<hashed_type (const value_type&, hashed_type)>;
 protected:
  using HashArray = std::array<int32_t, N>;
  HashFunc hashFunc_;
  hashed_type seed_;

  void addToHash(HashArray &data, const value_type &item) {
    std::bitset<N> hashBits = hashFunc_(item, seed_);
    for (size_t i = 0; i < N; i++) {
      if (hashBits[i] == 0) {
        data[i] -= 1;
      } else {
        data[i] += 1;
      }
    }
  }

 public:
  SimHasher(HashFunc hashFunc, hashed_type seed)
    : hashFunc_(hashFunc), seed_(seed) {}

  template<typename TCollection>
  hashed_type hash(const TCollection &valueCollection) {
    HashArray hashAccumulator;
    for (size_t i = 0; i < N; i++) {
      hashAccumulator[i] = 0;
    }
    for (auto &item: valueCollection) {
      addToHash(hashAccumulator, item);
    }
    std::bitset<N> hashBits;
    for (size_t i = 0; i < N; i++) {
      if (hashAccumulator[i] >= 0) {
        hashBits[i] = 1;
      }
    }
    return hashBits.to_ullong();
  }

  template<template<class...> class TTmpl, typename ...Types>
  hashed_type hash(const TTmpl<TValue, Types...> &valueCollection) {
    return hash<TTmpl<TValue, Types...>>(valueCollection);
  }
};

}} // score::hashing
