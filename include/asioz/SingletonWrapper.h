#pragma once

namespace asioz {

template<typename T>
class SingletonWrapper {
 public:
  using value_type = T;
 protected:
  T underlyingInstance_;
  SingletonWrapper(){}
  static SingletonWrapper<T>* getSingletonWrapperInstance() {
    static SingletonWrapper<T> instance;
    return &instance;
  }
 public:
  static T* get() {
    return &getSingletonWrapperInstance()->underlyingInstance_;
  }
};

} // asioz
