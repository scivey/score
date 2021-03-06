#pragma once
#include <memory>

namespace score { namespace mem {

template<typename T>
std::unique_ptr<T> makeUnique() {
  return std::unique_ptr<T>(new T);
}

template<typename T, typename ...Args>
std::unique_ptr<T> makeUnique(Args&& ...args) {
  return std::unique_ptr<T>(new T(std::forward<Args...>(args...)));
}


}} // score::mem
