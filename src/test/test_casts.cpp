#include <gtest/gtest.h>

struct Point1 {
  int x {0}, y {0};
};

struct Point2 {
  int x {0}, y {0};
};


TEST(TestCasts, TestReinterpretCast) {
  Point1 p1;
  Point1 *ptr1 = &p1;
  EXPECT_TRUE(true);
}

