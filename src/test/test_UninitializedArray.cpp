#include "aliens/UninitializedArray.h"
#include <gtest/gtest.h>

using NoInit = UninitializedArray<int, 32>;

TEST(TestUninitializedArray, TestSanity1) {
  NoInit something;
  EXPECT_EQ(0, something.size());
  EXPECT_TRUE(something.initialize(5));
  EXPECT_EQ(1, something.size());
  EXPECT_FALSE(something.initialize(5));
  EXPECT_EQ(1, something.size());
  EXPECT_TRUE(something.initialize(7));
  EXPECT_EQ(2, something.size());
}