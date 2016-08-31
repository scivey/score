#pragma once
#include <mutex>
#include <thread>
#include <map>
#include <set>
#include <glog/logging.h>
#include "aliens/ThreadLocalPtr.h"
#include "aliens/Counter.h"
#include "aliens/macros.h"
#include "aliens/SingletonWrapper.h"

namespace aliens { namespace test_support {

template<size_t NIdent>
class Noisy {
 public:
  static const size_t ID = NIdent;
  using StaticCounter = SingletonWrapper<Counter<NIdent>>;

  class Report {
   public:
    using id_set = std::set<size_t>;
    using id_counts = std::map<size_t, size_t>;
   protected:
    mutable std::mutex mutex_;
    id_set created_;
    id_set destroyed_;
    id_set outstanding_;
    id_counts moves_;
   public:
    void reset() {
      std::lock_guard<std::mutex> lg {mutex_};
      created_.clear();
      destroyed_.clear();
      outstanding_.clear();
      moves_.clear();
    }
    void markCreated(size_t id) {
      std::lock_guard<std::mutex> lg {mutex_};
      created_.insert(id);
      outstanding_.insert(id);
    }
    void markDestroyed(size_t id) {
      std::lock_guard<std::mutex> lg {mutex_};
      ACHECK(outstanding_.count(id) > 0);
      outstanding_.erase(id);
      destroyed_.insert(id);
    }
    void markMoved(size_t id) {
      auto found = moves_.find(id);
      if (found == moves_.end()) {
        moves_.insert(std::make_pair(id, 1));
      } else {
        moves_.at(id) += 1;
      }
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
    const id_set& getCreated() const {
      std::lock_guard<std::mutex> lg {mutex_};
      return created_;
    }
    const id_set& getOutstanding() const {
      std::lock_guard<std::mutex> lg {mutex_};
      return created_;
    }
    const id_set& getDestroyed() const {
      std::lock_guard<std::mutex> lg {mutex_};
      return created_;
    }
    const id_counts& getMoves() const {
      std::lock_guard<std::mutex> lg {mutex_};
      return moves_;
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
  static const typename Report::id_counts& getMoves() {
    return getReport()->getMoves();
  }
  static const typename Report::id_set& getDestroyed() {
    return getReport()->getDestroyed();
  }
  static const typename Report::id_set& getCreated() {
    return getReport()->getCreated();
  }
  static const typename Report::id_set& getOutstanding() {
    return getReport()->getOutstanding();
  }
  static size_t nCreated() {
    return getReport()->nCreated();
  }
  static size_t nMovedIds() {
    return getReport()->getMoves().size();
  }
  static size_t nTotalMoves() {
    auto moves = getMoves();
    size_t accum {0};
    for (auto &oneMove: moves) {
      accum += oneMove.second;
    }
    return accum;
  }

  static size_t nDestroyed() {
    return getReport()->nDestroyed();
  }
  static size_t nOutstanding() {
    return getReport()->nOutstanding();
  }
  static void resetReport() {
    getReport()->reset();
  }
  bool hasId() {
    return id_ != kDefaultID;
  }
  Noisy(Noisy &&other): id_(other.id_) {
    other.id_ = kDefaultID;
    getReport()->markMoved(id_);
  }
  Noisy& operator=(Noisy &&other) {
    maybeMarkDestroyed();
    id_ = other.id_;
    getReport()->markMoved(id_);
    other.id_ = kDefaultID;
    return *this;
  }
  size_t getId() const {
    return id_;
  }
  Noisy(): id_(StaticCounter::get()->get()) {
    getReport()->markCreated(id_);
  }
  void speak() {
    LOG(INFO) << "Noisy<" << NIdent << "> : id='" << id_ << "'";
  }
  ~Noisy() {
    maybeMarkDestroyed();
  }
};


}} // aliens::test_support