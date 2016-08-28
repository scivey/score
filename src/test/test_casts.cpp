#include <gtest/gtest.h>
#include <memory>
#include <glog/logging.h>
#include <spdlog/spdlog.h>
using namespace std;

struct Point1 {
  int x {0}, y {0};
};

struct Point2 {
  int x {0}, y {0};
};


class Something : public enable_shared_from_this<Something> {
 protected:
  size_t value_ {0};

 public:
  Something(size_t val): value_(val) {}
  size_t getValue() const {
    return value_;
  }
};


class XX {
 public:
  enum class State {
    A = 1, B = 2, C =3, NOTHING = 4
  };
 protected:
  mutable State state_ {State::NOTHING};
 public:
  State getState() {
    return state_;
  }
  void run() & {
    state_ = State::A;
  }
  void run() const& {
    state_ = State::B;
  }
  void run() && {
    state_ = State::C;
  }
  void reset() {
    state_ = State::NOTHING;
  }
};

TEST(TestXX, TestWorks1) {
  XX thing;
  EXPECT_EQ(XX::State::NOTHING, thing.getState());
  auto func = [](XX& plainRef) {
    plainRef.run();
  };
  func(thing);
  EXPECT_EQ(XX::State::A, thing.getState());
}

TEST(TestXX, TestWorks2) {
  XX thing;
  EXPECT_EQ(XX::State::NOTHING, thing.getState());
  auto func = [](const XX& constRef) {
    constRef.run();
  };
  func(thing);
  EXPECT_EQ(XX::State::B, thing.getState());
}
