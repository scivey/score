#pragma once

#include <memory>

namespace score { namespace encodings {

template<typename T>
struct FreeDeleter {
  void operator()(T* ptr) const {
    free(ptr);
  }
};

struct FreeVoidDeleter {
  void operator()(void* ptr) const {
    free(ptr);
  }
};

template<typename T>
class HeapBufferHandle {
 protected:
  using BuffPtr = std::unique_ptr<T, FreeVoidDeleter>;
  BuffPtr buff_;
  size_t capacity_{0};
  HeapBufferHandle(const HeapBufferHandle&) = delete;
  HeapBufferHandle& operator=(const HeapBufferHandle&) = delete;
  HeapBufferHandle(){}
 public:
  HeapBufferHandle(HeapBufferHandle&&) = default;
  HeapBufferHandle& operator=(HeapBufferHandle&&) = default;
  static HeapBufferHandle create(size_t capacity) {
    HeapBufferHandle instance;
    instance.capacity_ = capacity;
    instance.buff_.reset((T*) malloc(capacity * sizeof(T)));
    return instance;
  }
  static HeapBufferHandle* createPtr(size_t capacity) {
    return new HeapBufferHandle(HeapBufferHandle::create(capacity));
  }
  bool isValid() const {
    return !!buff_;
  }
  operator bool() const {
    return isValid();
  }
  size_t totalCapacity() {
    return capacity_;
  }
  T* data() {
    return buff_.get();
  }
};


}} // score::encodings
