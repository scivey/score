#include <gtest/gtest.h>
#include <sstream>
#include <memory>
#include <string>
#include "score/func/Function.h"

namespace func = score::func;
using namespace std;

std::string stringOfInt(int x) {
  std::ostringstream oss;
  oss << x;
  return oss.str();
}

using ITSFunc = func::Function<int, string>;

TEST(TestFunction, TestGoodness1) {
  ITSFunc fn;
  EXPECT_FALSE(fn.good());
  EXPECT_TRUE(!fn);
  fn = [](int x) {
    return "sure";
  };
  EXPECT_TRUE(fn.good());
  EXPECT_FALSE(!fn);
}

TEST(TestFunction, TestGoodness2) {
  ITSFunc fn([](int x) {
    return "yeah";
  });
  EXPECT_TRUE(fn.good());
  EXPECT_FALSE(!fn);
}

TEST(TestFunction, TestLambda1) {
  ITSFunc fn([](int x) {
    return stringOfInt(x);
  });
  EXPECT_EQ("7", fn(7));
}


class IntConverter {
 protected:
  size_t id_ {0};
 public:
  IntConverter(size_t id): id_(id){}
  size_t getId() const {
    return id_;
  }
  std::string convert(int x) {
    return stringOfInt(x);
  }
};

TEST(TestFunction, TestCapturingLambda) {
  auto converter = std::make_shared<IntConverter>(26);
  EXPECT_EQ(26, converter->getId());
  ITSFunc fn([converter](int x) {
    EXPECT_EQ(26, converter->getId());
    return converter->convert(x);
  });
  EXPECT_EQ("7", fn(7));
}

TEST(TestFunction, TestFreeFunc) {
  ITSFunc fn(stringOfInt);
  EXPECT_EQ("7", fn(7));
}


