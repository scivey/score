#include <gtest/gtest.h>
#include <functional>
#include <atomic>
#include "score/Optional.h"

using namespace std;
using score::Optional;

TEST(TestOptional, TestWorks1) {
  Optional<int> x {17};
  EXPECT_EQ(17, x.value());
  EXPECT_TRUE(x.hasValue());
}

TEST(TestOptional, TestWorks2) {
  Optional<int> x;
  EXPECT_FALSE(x.hasValue());
}

TEST(TestOptional, TestMoving) {
  Optional<int> x;
  EXPECT_FALSE(x.hasValue());
  Optional<int> y {26};
  EXPECT_TRUE(y.hasValue());
  EXPECT_EQ(26, y.value());
  x.assign(std::move(y));
  EXPECT_TRUE(x.hasValue());
  EXPECT_FALSE(y.hasValue());
  EXPECT_EQ(26, x.value());
}
