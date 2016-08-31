#pragma once
#include <memory>
#include "score/mem/LSharedPtr.h"

namespace score {

template<typename T>
class PointerFactory {
 public:
  template<typename ...Args>
  static std::shared_ptr<T> createShared(Args&&... args) {
    return std::shared_ptr<T>(
      T::createPtr(std::forward<Args>(args)...)
    );
  }

  template<typename ...Args>
  static T* createRaw(Args&&... args) {
    return T::createPtr(std::forward<Args>(args)...);
  }

  template<typename ...Args>
  static std::unique_ptr<T> createUnique(Args&&... args) {
    return std::unique_ptr<T>(
      T::createPtr(std::forward<Args>(args)...)
    );
  }

  template<typename ...Args>
  static T create(Args&&... args) {
    return T::create(std::forward<Args>(args)...);
  }

  template<typename ...Args>
  static score::mem::LSharedPtr<T> createLShared(Args&&... args) {
    auto instance = T::create(std::forward<Args>(args)...);
    return score::mem::makeLShared<T>(std::move(instance));
  }
};

}