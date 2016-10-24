#include <gtest/gtest.h>
#include <vector>
#include "score/iter/IterTrailer.h"
#include "score/iter/util.h"

using namespace std;
using namespace score::iter;


TEST(TestIterTrailer, TestWorks) {
  std::vector<int> nums {1, 5, 7, 9};
  std::vector<std::pair<int, int>> expected {
    {1, 5},
    {5, 7},
    {7, 9}
  };
  std::vector<std::pair<int, int>> actual;
  auto trailer = iterTrailing(nums);
  drainIterable(trailer, actual);
  EXPECT_EQ(expected, actual);
}
