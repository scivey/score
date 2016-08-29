#pragma once
#include <memory>
#include "aliens/mem/LSharedPtr.h"

namespace aliens {

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
  static aliens::mem::LSharedPtr<T> createLShared(Args&&... args) {
    auto instance = T::create(std::forward<Args>(args)...);
    return aliens::mem::makeLShared<T>(std::move(instance));
  }
};

}