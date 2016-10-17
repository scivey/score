#pragma once
#include <bitset>
#include <type_traits>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <cstdio>
#include <cstring>
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


template<typename T>
void mzero(T* ptr) {
  memset((void*) ptr, 0, sizeof(T));
}

template<typename T>
void mzero(T& ref) {
  mzero<T>(&ref);
}

template<typename T>
struct underlying_type {
  using input_type = T;
  using type = typename std::remove_pointer<
    typename std::remove_reference<T>::type
  >::type;

  struct size {
    static const size_t value = sizeof(type);
  };
};

template<typename T, typename TUnder = typename underlying_type<T>::type>
void copyRef(TUnder& dest, const TUnder& src) {
  memcpy(&dest, &src, underlying_type<T>::size::value);
}


template<typename T>
struct destructor_type {
  using type = std::function<void(T*)>;
};

template<typename T>
struct unique_destructor_ptr {
  using type = std::unique_ptr<T, typename destructor_type<T>::type>;
};

template<typename T, typename TCallable>
auto asDestructorPtr(T *ptr, TCallable &&callable) -> typename unique_destructor_ptr<T>::type {
  using uniq_t = typename unique_destructor_ptr<T>::type;
  return uniq_t {ptr, std::forward<TCallable>(callable)};
}

template<typename T, typename TCallable>
auto asDestructorPtr(T *ptr, const TCallable &callable) -> typename unique_destructor_ptr<T>::type {
  using uniq_t = typename unique_destructor_ptr<T>::type;
  return uniq_t {ptr, callable};
}

template<typename T, typename... Args>
std::unique_ptr<T> makeUnique(Args&&... args) {
  return std::unique_ptr<T> {
    new T {std::forward<Args>(args)...}
  };
}

template<typename T, template<class...> class TSmartPtr, typename ...Types>
TSmartPtr<T> createSmart(Types&&... args) {
  return TSmartPtr<T> { T::createNew(std::forward<Types>(args)...) };
}

template<typename T, typename ...Types>
std::unique_ptr<T> createUnique(Types&&... args) {
  return createSmart<T, std::unique_ptr, Types...>(
    std::forward<Types>(args)...
  );
}

template<typename T, typename ...Types>
std::shared_ptr<T> createShared(Types&&... args) {
  return createSmart<T, std::shared_ptr, Types...>(
    std::forward<Types>(args)...
  );
}


}} // score::util
