#pragma once
#include <tuple>
#include <array>
#include <sstream>
#include "score/hashing/util.h"
#include "score/macros.h"
#include "score/prettyprint/prettyprint_predef.h"

namespace score { namespace nlp {

template<typename T, size_t NGramSize>
struct NGram {
  using value_type = T;
  static const size_t N = NGramSize;
  std::array<value_type, N> codepoints;
  NGram() {
    for (size_t i = 0; i < NGramSize; i++) {
      codepoints[i] = 0;
    }
  }
  NGram(std::initializer_list<value_type> init) {
    SDCHECK(init.size() == NGramSize);
    size_t idx = 0;
    for (auto item: init) {
      codepoints[idx] = item;
      idx++;
    }
  }
  bool operator==(const NGram &other) const {
    for (size_t i = 0; i < N; i++) {
      if (codepoints[i] != other.codepoints[i]) {
        return false;
      }
    }
    return true;
  }
  bool operator<(const NGram &other) const {
    for (size_t i = 0; i < N; i++) {
      auto cp = codepoints[i];
      auto otherCp = other.codepoints[i];
      if (cp < otherCp) return true;
      if (cp == otherCp) continue;
      return false;
    }
    return false;
  }
  value_type& operator[](size_t idx) {
    return codepoints[idx];
  }
  value_type operator[](size_t idx) const {
    return codepoints[idx];
  }
  value_type& at(size_t idx) {
    return codepoints[idx];
  }
  value_type at(size_t idx) const {
    return codepoints[idx];
  }
};

}} // score::nlp




namespace score { namespace prettyprint {

template<typename T, size_t NGramSize>
struct PrettyPrinter<score::nlp::NGram<T, NGramSize>> {
  using arg_type = score::nlp::NGram<T, NGramSize>;
  static const size_t N = NGramSize;
  static void pprint(std::ostringstream &oss, const arg_type &arg) {
    oss << "NGram[" << NGramSize << "] : (";
    for (size_t i = 0; i <= N - 1; i++) {
      prettyPrint(oss, arg.codepoints[i]);
      if (i < N - 1) {
        oss << ", ";
      }
    }
    oss << ")";
  }
};

}} // score::prettyprint


namespace std {

template<typename T, size_t NGramSize>
struct hash<score::nlp::NGram<T, NGramSize>> {
  using arg_type=score::nlp::NGram<T, NGramSize>;
  static const size_t N = NGramSize;
  size_t operator()(const arg_type &arg) const {
    size_t seed = std::hash<T>()(arg.codepoints[0]);
    for (size_t i = 1; i < N; i++) {
      seed = score::hashing::hashCombine(seed, std::hash<T>()(arg.codepoints[i]));
    }
    return seed;
  }
};

} // std
