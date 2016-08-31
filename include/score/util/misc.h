#pragma once
#include <bitset>
#include <type_traits>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <sstream>
#include <functional>
#include "score/macros.h"

namespace score { namespace util {

template<typename T>
void voidDeleter(void *data) {
  T* ptr = (T*) data;
  delete ptr;
}

template<typename T, size_t N = (sizeof(T) * 8),
  typename Ignore = typename std::enable_if<std::is_scalar<T>::value>::type>
size_t hammingDistance(T t1, T t2) {
  std::bitset<N> b1 = t1;
  std::bitset<N> b2 = t2;
  size_t acc = 0;
  for (size_t i = 0; i < N; i++) {
    if (b1[i] != b2[i]) {
      acc++;
    }
  }
  return acc;
}

template<typename T1, typename T2>
std::set<T1> keySet(const std::map<T1, T2> &aMap) {
  std::set<T1> result;
  for (auto &item: aMap) {
    result.insert(item.first);
  }
  return result;
}

template<typename T1, typename T2>
std::vector<T1> keyVec(const std::map<T1, T2> &aMap) {
  std::vector<T1> result;
  result.reserve(aMap.size());
  for (auto &item: aMap) {
    result.push_back(item.first);
  }
  return result;
}



template<typename T>
bool always(const T &) {
  return true;
}

template<typename T1, typename T2>
const T1& largestKey(const std::map<T1, T2> &aMap) {
  SDCHECK(aMap.size() > 0);
  return aMap.rbegin()->first;
}

template<typename T1>
const T1& largestKey(const std::set<T1> &aSet) {
  SDCHECK(aSet.size() > 0);
  return *aSet.rbegin();
}

template<typename T1>
const T1& smallestKey(const std::set<T1> &aSet) {
  SDCHECK(aSet.size() > 0);
  return *aSet.begin();
}

}} // score::util
