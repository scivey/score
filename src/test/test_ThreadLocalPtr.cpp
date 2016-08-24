#include <gtest/gtest.h>
#include <mutex>
#include <thread>

#include "aliens/ThreadLocalPtr.h"
#include "aliens/Counter.h"
#include "aliens/SingletonWrapper.h"
using aliens::ThreadLocalPtr;
using aliens::SingletonWrapper;
using aliens::Counter;

using StaticCounter = SingletonWrapper<Counter<13>>;

class Noisy {
 public:
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

TEST(TestLocalPtr, TestSimple) {
  auto rep = Noisy::getReport();
  rep->reset();
  EXPECT_EQ(0, rep->nCreated());
  {
    auto localPtr = aliens::ThreadLocalPtr<Noisy>::create();
    {
      EXPECT_EQ(0, rep->nCreated());
      auto x = localPtr.get();
      EXPECT_EQ(1, rep->nCreated());
      auto y = localPtr.get();
      EXPECT_EQ(x, y);
      EXPECT_EQ(1, rep->nCreated());
      std::thread t1([&localPtr]() {
        auto rep = Noisy::getReport();
        auto z1 = localPtr.get();
        EXPECT_EQ(2, rep->nCreated());
        auto z2 = localPtr.get();
        EXPECT_EQ(z1, z2);
        EXPECT_EQ(2, rep->nCreated());
        EXPECT_EQ(2, rep->nOutstanding());
        EXPECT_EQ(0, rep->nDestroyed());
      });
      t1.join();
      EXPECT_EQ(2, rep->nCreated());
      EXPECT_EQ(1, rep->nOutstanding());
      EXPECT_EQ(1, rep->nDestroyed());
    }
    {
      EXPECT_EQ(2, rep->nCreated());
      EXPECT_EQ(1, rep->nOutstanding());
      EXPECT_EQ(1, rep->nDestroyed());
      std::thread t1([&localPtr]() {
        auto rep = Noisy::getReport();
        auto z1 = localPtr.get();
        EXPECT_EQ(3, rep->nCreated());
        auto z2 = localPtr.get();
        EXPECT_EQ(z1, z2);
        EXPECT_EQ(3, rep->nCreated());
        EXPECT_EQ(2, rep->nOutstanding());
        EXPECT_EQ(1, rep->nDestroyed());
      });
      t1.join();
      EXPECT_EQ(3, rep->nCreated());
      EXPECT_EQ(1, rep->nOutstanding());
      EXPECT_EQ(2, rep->nDestroyed());
    }
    EXPECT_EQ(3, rep->nCreated());
    EXPECT_EQ(2, rep->nDestroyed());
    EXPECT_EQ(1, rep->nOutstanding());
  }
  EXPECT_EQ(3, rep->nCreated());
  EXPECT_EQ(1, rep->nOutstanding());
}
