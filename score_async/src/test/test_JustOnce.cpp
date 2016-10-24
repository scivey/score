#include <gtest/gtest.h>
#include "score_async/JustOnce.h"

TEST(TestJustOnce, TestWorks) {
  int x = 0;
  score::async::JustOnce something {[&x]() {
    x++;
  }};
  EXPECT_EQ(0, x);
  something();
  EXPECT_EQ(1, x);
  for (size_t i = 0; i < 10; i++) {
    something();
    EXPECT_EQ(1, x);
  }
}