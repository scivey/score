#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "score/prettyprint/prettyprint.h"

using namespace std;
using namespace score::prettyprint;


TEST(TestPrettyPrint, TestNotNonsensical) {
  vector<int> nums {5, 7, 3};
  string result = prettyPrint(nums);
  vector<char> expected {'5', '7', '3'};
  for (auto c: expected) {
    EXPECT_TRUE(result.find(c) != string::npos);
  }
}
