#pragma once
#include <mutex>
#include <thread>
#include "aliens/ThreadLocalPtr.h"
#include "aliens/Counter.h"
#include "aliens/SingletonWrapper.h"

namespace aliens { namespace test_support {

template<size_t NIdent>
class Noisy {
 public:
  static const size_t ID = NIdent;
  using StaticCounter = SingletonWrapper<Counter<NIdent>>;

  class Report {
   protected:
    std::mutex mutex_;
    std::set<size_t> created_;
    std::set<size_t> destroyed_;
    std::set<size_t> outstanding_;
   public:
    void reset() {
      std::lock_guard<std::mutex> lg {mutex_};
      created_.clear();
      destroyed_.clear();
      outstanding_.clear();
    }
    void markCreated(size_t id) {
      std::lock_guard<std::mutex> lg {mutex_};
      created_.insert(id);
      outstanding_.insert(id);
    }
    void markDestroyed(size_t id) {
      std::lock_guard<std::mutex> lg {mutex_};
      CHECK(outstanding_.count(id) > 0);
      outstanding_.erase(id);
      destroyed_.insert(id);
    }
    size_t nCreated() {
      std::lock_guard<std::mutex> lg {mutex_};
      return created_.size();
    }
    size_t nDestroyed() {
      std::lock_guard<std::mutex> lg {mutex_};
      return destroyed_.size();
    }
    size_t nOutstanding() {
      std::lock_guard<std::mutex> lg {mutex_};
      return outstanding_.size();
    }
  };
  static Report* getReport() {
    return SingletonWrapper<Report>::get();
  }

 protected:
  static const size_t kDefaultID = 99999999;
  size_t id_ {kDefaultID};
  Noisy(const Noisy&) = delete;
  Noisy& operator=(const Noisy&) = delete;
  void maybeMarkDestroyed() {
    if (hasId()) {
      getReport()->markDestroyed(id_);
      id_ = kDefaultID;
    }
  }
 public:
  bool hasId() {
    return id_ != kDefaultID;
  }
  Noisy(Noisy &&other): id_(other.id_) {
    other.id_ = kDefaultID;
  }
  Noisy& operator=(Noisy &&other) {
    maybeMarkDestroyed();
    id_ = other.id_;
    other.id_ = kDefaultID;
    return *this;
  }
  Noisy(): id_(StaticCounter::get()->get()) {
    getReport()->markCreated(id_);
  }
  ~Noisy() {
    maybeMarkDestroyed();
  }
};


}} // aliens::test_support