#pragma once

/* Shamelessly stolen from Seastar and adapted to c++11.
 * LSharedPtr is a non-threadsafe version of std::shared_ptr.
 * When we know a given resource will only be used in a single
 * thread, using this class avoids the extra cache traffic
 * of std::shared_ptr's atomic operations.
*/

#include "aliens/vendored/seastar/core/shared_ptr.hh"

namespace aliens { namespace mem {

template<typename T>
using LSharedPtr = aliens::vendored::seastar::shared_ptr<T>;

template<typename T, typename ...Args>
LSharedPtr<T> makeLShared(Args&&... args) {
  return aliens::vendored::seastar::make_shared<T>(
    std::forward<Args...>(args...)
  );
}

template<typename T>
LSharedPtr<T> makeLShared() {
  return aliens::vendored::seastar::make_shared<T>();
}

template<typename T>
LSharedPtr<T> makeLShared(T&& instance) {
  return aliens::vendored::seastar::make_shared<T>(
    std::forward<T>(instance)
  );
}

}};
