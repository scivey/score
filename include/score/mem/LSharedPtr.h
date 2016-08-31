#pragma once

/* Shamelessly stolen from Seastar and adapted to c++11.
 * LSharedPtr is a non-threadsafe version of std::shared_ptr.
 * When we know a given resource will only be used in a single
 * thread, using this class avoids the extra cache traffic
 * of std::shared_ptr's atomic operations.
*/

#include "score/vendored/seastar/core/shared_ptr.hh"

namespace score { namespace mem {

template<typename T>
using LSharedPtr = score::vendored::seastar::shared_ptr<T>;

template<typename T, typename ...Args>
LSharedPtr<T> makeLShared(Args&&... args) {
  return score::vendored::seastar::make_shared<T>(
    std::forward<Args...>(args...)
  );
}

template<typename T>
LSharedPtr<T> makeLShared() {
  return score::vendored::seastar::make_shared<T>();
}

template<typename T>
LSharedPtr<T> makeLShared(T&& instance) {
  return score::vendored::seastar::make_shared<T>(
    std::forward<T>(instance)
  );
}

}};
