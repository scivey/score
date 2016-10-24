#pragma once
#include "hashing/HashFamily.h"
#include <bitset>
#include <array>
#include <functional>
#include <random>

namespace score { namespace hashing {

template<typename TValue, typename THash, size_t FilterSize, size_t NumHashes>
class BloomFilter {
 public:
  static const size_t NBits = FilterSize;
  static const size_t NHashes = NumHashes;
  using value_type = TValue;
  using hashed_type = THash;
  using hash_family_type = HashFamily<value_type, hashed_type, NumHashes>;
  using hash_func_type = typename hash_family_type::hash_func_type;
  using seed_array_type = std::array<hashed_type, NHashes>;
 protected:
  hash_family_type *hashFamily_;
  std::bitset<NBits> bits_;

 public:

  BloomFilter(hash_family_type *hashFamily)
    : hashFamily_(hashFamily) {}

  void insert(const value_type &value) {
    for (auto hashFunc: *hashFamily_) {
      size_t bitIdx = hashFunc(value) % NBits;
      bits_[bitIdx] = 1;
    }
  }

  bool has(const value_type &value) {
    for (auto hashFunc: *hashFamily_) {
      size_t bitIdx = hashFunc(value) % NBits;
      if (bits_[bitIdx] != 1) {
        return false;
      }
    }
    return true;
  }

  bool insertIfMissing(const value_type &value) {
    bool missing = false;
    for (auto hashFunc: *hashFamily_) {
      size_t bitIdx = hashFunc(value) % NBits;
      if (bits_[bitIdx] == 0) {
        missing = true;
      }
      bits_[bitIdx] = 1;
    }
    return missing;
  }
};

}} // score::hashing
