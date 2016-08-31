#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "aliens/io/string_utils.h"

using namespace std;
using namespace aliens::io;

TEST(TestStringUtils, TestJoinWith) {
  vector<string> parts {"one", "two", "three"};
  string expected = "one|two|three";
  string result = joinWith("|", parts);
  EXPECT_EQ(expected, result);
}

