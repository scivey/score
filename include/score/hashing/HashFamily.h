#pragma once

#include <bitset>
#include <array>
#include <functional>
#include <random>

namespace score { namespace hashing {

template<typename TValue, typename THash, size_t NumHashes>
class HashFamily {
 public:
  using value_type = TValue;
  using hashed_type = THash;
  static const size_t N = NumHashes;
  using seed_array_type = std::array<hashed_type, N>;
  using hash_func_type = std::function<hashed_type (const value_type&, hashed_type)>;
 protected:
  hash_func_type hashFunc_;
  seed_array_type seeds_;
  hashed_type callNth(const value_type &value, size_t idx) {
    return hashFunc_(value, seeds_[idx]);
  }

 public:
  HashFamily(hash_func_type hashFunc, const seed_array_type &seeds): hashFunc_(hashFunc) {
    for (size_t i = 0; i < N; i++) {
      seeds_[i] = seeds[i];
    }
  }

  static HashFamily create(hash_func_type hashFunc, size_t seed) {
    std::mt19937 engine {seed};
    hashed_type nothing = 0;
    hashed_type everything = ~nothing;
    std::uniform_int_distribution<hashed_type> dist(nothing, everything);
    seed_array_type seeds;
    for (size_t i = 0; i < N; i++) {
      seeds[i] = dist(engine);
    }
    return HashFamily(hashFunc, seeds);
  }

  static HashFamily create(hash_func_type hashFunc) {
    std::random_device rd;
    return HashFamily::create(rd());
  }

  class Callable {
   protected:
    HashFamily *parent_;
    size_t seedIndex_;
   public:
    Callable(HashFamily *parent, size_t seedIndex)
      : parent_(parent), seedIndex_(seedIndex){}
    hashed_type operator()(const value_type &ref) {
      return parent_->callNth(ref, seedIndex_);
    }
  };

  friend class Callable;

  class Iterator {
   protected:
    HashFamily *parent_;
    size_t seedIndex_;
   public:
    Iterator(HashFamily *parent, size_t seedIndex = 0)
      : parent_(parent), seedIndex_(seedIndex) {}

    Callable operator*() {
      return Callable(parent_, seedIndex_);
    }

    Iterator& operator++() {
      seedIndex_++;
      return *this;
    }

    Iterator operator++(int) {
      Iterator result(parent_, seedIndex_);
      ++result;
      return result;
    }

    bool operator==(const Iterator &other) const {
      return seedIndex_ == other.seedIndex_;
    }

    bool operator!=(const Iterator &other) const {
      return seedIndex_ != other.seedIndex_;
    }

  };

  friend class Iterator;

  Iterator begin() {
    return Iterator(this, 0);
  }

  Iterator end() {
    return Iterator(this, N);
  }

};

}} // score::hashing
