#pragma once

#include <pthread.h>
#include <memory>
#include "aliens/exceptions/macros.h"

namespace aliens {


template<typename T>
void destructor(void *ptr) {
  auto instance = (T*) ptr;
  delete instance;
}


template<typename T>
class ThreadLocalPtr {
 public:
  using value_type = T;
 protected:
  pthread_key_t key_ {0};
  ThreadLocalPtr(){}
  ThreadLocalPtr(const ThreadLocalPtr &) = delete;
  ThreadLocalPtr& operator=(const ThreadLocalPtr&) = delete;
 public:
  ThreadLocalPtr(ThreadLocalPtr &&other): key_(other.key_) {
    other.key_ = 0;
  }
  ThreadLocalPtr& operator=(ThreadLocalPtr &&other) {
    pthread_key_t temp = key_;
    key_ = other.key_;
    other.key_ = temp;
    return *this;
  }
  static ThreadLocalPtr create() {
    ThreadLocalPtr instance;
    ACHECK(pthread_key_create(&instance.key_, destructor<T>) == 0);
    return instance;
  }
  operator bool() const {
    return good();
  }
  bool good() const {
    return key_ != 0;
  }
  T* get() {
    ADCHECK(good());
    auto instance = pthread_getspecific(key_);
    if (instance == nullptr) {
      T* newInstance = new T;
      instance = (void*) newInstance;
      pthread_setspecific(key_, instance);
    }
    return (T*) instance;
  }
  T* operator->() {
    return get();
  }
  T& operator*() {
    auto instance = get();
    return *instance;
  }
  void clear() {
    if (good()) {
      pthread_key_delete(key_);
      key_ = 0;
    }
  }
  ~ThreadLocalPtr() {
    clear();
  }
};

} // aliens