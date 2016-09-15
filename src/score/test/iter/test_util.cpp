#include <gtest/gtest.h>
#include <vector>
#include "score/iter/util.h"

using namespace std;
using namespace score::iter;

TEST(TestIterUtil, TestDrainIterable1) {
  vector<int> nums {5, 7, 3};
  vector<int> actual;
  drainIterable(nums, actual);
  vector<int> expected {5, 7, 3};
  EXPECT_EQ(expected, actual);
}

TEST(TestIterUtil, TestDrainIterable2) {
  vector<int> nums;
  vector<int> actual;
  drainIterable(nums, actual);
  vector<int> expected;
  EXPECT_EQ(expected, actual);
}
