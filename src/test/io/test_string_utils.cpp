#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "score/io/string_utils.h"

using namespace std;
using namespace score::io;

TEST(TestStringUtils, TestJoinWith) {
  vector<string> parts {"one", "two", "three"};
  string expected = "one|two|three";
  string result = joinWith("|", parts);
  EXPECT_EQ(expected, result);
}

