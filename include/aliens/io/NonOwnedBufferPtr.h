#pragma once
#include <memory>
#include <string>

namespace aliens { namespace io {

template<typename T>
class BaseNonOwnedBufferPtr {
 public:
  using value_type = T;
 protected:
  T *buff_ {nullptr};
  size_t buffLen_ {0};
 public:
  BaseNonOwnedBufferPtr(T *buff, size_t buffLen)
    : buff_(buff), buffLen_(buffLen) {}
  size_t size() const {
    return buffLen_;
  }
  T* data() {
    return buff_;
  }
  void* vdata() {
    return (void*) buff_;
  }

  // depends on being able to cast to char
  std::string copyToString() {
    std::string result {""};
    if (!buff_) {
      return result;
    }
    auto charPtr = (char*) data();
    for (size_t i = 0; i < buffLen_; i++) {
      char *current = charPtr + i;
      result.push_back(*current);
    }
    return result;
  }
};

using NonOwnedBufferPtr = BaseNonOwnedBufferPtr<char>;

}} // aliens::io
