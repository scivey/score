#include <gtest/gtest.h>
#include "score/format.h"

TEST(TestFormat, TestSanity) {
  std::string result = score::format("{} {} {}", 5, "dog", 7);
  EXPECT_EQ("5 dog 7", result);
}
