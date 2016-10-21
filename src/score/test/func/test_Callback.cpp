#include <gtest/gtest.h>
#include <sstream>
#include <memory>
#include <string>
#include <atomic>
#include "score/func/Callback.h"

namespace func = score::func;
using namespace std;

using IntCB = func::Callback<int>;

TEST(TestCallback, TestGoodness1) {
  IntCB fn;
  EXPECT_FALSE(fn.good());
  EXPECT_TRUE(!fn);
  int calledWith {0};
  fn = [&calledWith](int x) {
    calledWith = x;
  };
  EXPECT_TRUE(fn.good());
  EXPECT_FALSE(!fn);
  fn(17);
  EXPECT_EQ(17, calledWith);
}

TEST(TestCallback, TestGoodness2) {
  int calledWith {0};
  IntCB fn([&calledWith](int x) {
    calledWith = x;
  });
  EXPECT_TRUE(fn.good());
  EXPECT_FALSE(!fn);
  fn(26);
  EXPECT_EQ(26, calledWith);
}

static std::atomic<int>& getStaticInt() {
  static std::atomic<int> staticInt {0};
  return staticInt;
}

static int loadStaticInt() {
  return getStaticInt().load();
}

static void setStaticInt(int x) {
  getStaticInt().store(x);
}

TEST(TestCallback, TestFreeFuncPreTest) {
  setStaticInt(17);
  EXPECT_EQ(17, loadStaticInt());
}

TEST(TestCallback, TestFreeFunc) {
  setStaticInt(0);
  IntCB fn {setStaticInt};
  EXPECT_EQ(0, loadStaticInt());
  fn(253);
  EXPECT_EQ(253, loadStaticInt());
}


