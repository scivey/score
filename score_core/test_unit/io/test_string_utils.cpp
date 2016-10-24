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

TEST(TestStringUtils, TestTrimWhitespace1) {
  string text {"  fish  bandit\t"};
  EXPECT_EQ("fish  bandit", trimWhitespace(text));
}
TEST(TestStringUtils, TestTrimWhitespace2) {
  string text {"fish  bandit"};
  EXPECT_EQ("fish  bandit", trimWhitespace(text));
}
TEST(TestStringUtils, TestTrimWhitespace3) {
  string text {"fish  bandit    "};
  EXPECT_EQ("fish  bandit", trimWhitespace(text));
}

