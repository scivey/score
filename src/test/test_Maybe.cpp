#include <gtest/gtest.h>
#include <functional>
#include <atomic>
#include "aliens/Maybe.h"

using namespace std;
using aliens::Maybe;

TEST(TestMaybe, TestWorks1) {
  Maybe<int> x {17};
  EXPECT_EQ(17, x.value());
  EXPECT_TRUE(x.hasValue());
}

TEST(TestMaybe, TestWorks2) {
  Maybe<int> x;
  EXPECT_FALSE(x.hasValue());
}

TEST(TestMaybe, TestMoving) {
  Maybe<int> x;
  EXPECT_FALSE(x.hasValue());
  Maybe<int> y {26};
  EXPECT_TRUE(y.hasValue());
  EXPECT_EQ(26, y.value());
  x.assign(std::move(y));
  EXPECT_TRUE(x.hasValue());
  EXPECT_FALSE(y.hasValue());
  EXPECT_EQ(26, x.value());
}
