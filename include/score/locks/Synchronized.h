#pragma once
#include <mutex>
#include <atomic>
#include <thread>

namespace score { namespace locks {

template<typename T>
class Synchronized {
 protected:
  T instance_;
  std::mutex mutex_;
 public:
  Synchronized(const Synchronized &other) = delete;
  Synchronized& operator=(const Synchronized &other) = delete;
 public:
  Synchronized() {}
  Synchronized(Synchronized&&) = default;
  Synchronized& operator=(Synchronized&&) = default;
  Synchronized(const T& val): instance_(val){}
  Synchronized(T &&val): instance_(std::move(val)) {}

  template<typename ...Args>
  Synchronized(Args&& ...args)
    : instance_(std::move(args...)) {}

  class PtrHandle {
   protected:
    std::atomic<Synchronized*> parent_ {nullptr};
    PtrHandle& operator=(const PtrHandle &other) = delete;
    PtrHandle(const PtrHandle& other) = delete;
    PtrHandle(Synchronized *parent) {
      parent_.store(parent);
      parent->mutex_.lock();
    }

    friend class Synchronized;
    void maybeClose() {
      auto parentPtr = parent_.load();
      if (parentPtr) {
        if (!parent_.compare_exchange_strong(parentPtr, nullptr)) {
          return;
        }
        parentPtr->mutex_.unlock();
      }
    }
   public:
    PtrHandle(PtrHandle &&other) {
      parent_.store(other.parent_);
      other.parent_.store(nullptr);
    }
    operator T* () {
      return parent_.load()->getInstancePtr();
    }
    T* get() {
      return parent_.load()->getInstancePtr();
    }
    PtrHandle& operator=(PtrHandle &&other) {
      maybeClose();
      for (;;) {
        auto otherParent = other.parent_.load();
        if (!otherParent) {
          parent_.store(nullptr);
          break;
        }
        if (other.parent_.compare_exchange_strong(otherParent, nullptr)) {
          parent_.store(otherParent);
          break;
        }
      }
      return *this;
    }
    T* operator->() {
      return get();
    }
    ~PtrHandle() {
      maybeClose();
    }
  };

 protected:
  T* getInstancePtr() {
    return &instance_;
  }
  void releaseLock() {
    mutex_.unlock();
  }
  friend class PtrHandle;

 public:

  PtrHandle getHandle() {
    return PtrHandle(this);
  }

  ~Synchronized() {
    mutex_.lock();
    mutex_.unlock();
  }
  // T* operator->() {

  //   // return getHandle().operator->();
  // }

};

}} // score::locks
