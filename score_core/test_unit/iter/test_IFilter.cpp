#include <gtest/gtest.h>
#include <vector>
#include "score/iter/IFilter.h"
#include "score/iter/util.h"


using namespace std;
using namespace score::iter;

bool isEven(int x) {
  return x % 2 == 0;
}

#define EXPECT_FILTERED_EQ(inVec, expected, func) do { \
    auto filterer = ifilter(func, inVec.begin(), inVec.end()); \
    decltype(expected) actual; \
    drainIterable(filterer, actual); \
    EXPECT_EQ(expected, actual); \
  } while(0)


TEST(TestIFilter, TestWorks1) {
  std::vector<int> nums {1, 2, 5, 6, 7, 8, 9, 16, 17, 22, 25};
  std::vector<int> expected {
    2, 6, 8, 16, 22
  };
  EXPECT_FILTERED_EQ(nums, expected, isEven);
}


TEST(TestIFilter, TestFirstTrue) {
  std::vector<int> nums {2, 5, 6, 7, 8, 9, 16, 17, 22, 25};
  std::vector<int> expected {
    2, 6, 8, 16, 22
  };
  EXPECT_FILTERED_EQ(nums, expected, isEven);
}

TEST(TestIFilter, TestLastTrue) {
  std::vector<int> nums {1, 2, 5, 6, 7, 8, 9, 16, 17, 22, 26};
  std::vector<int> expected {
    2, 6, 8, 16, 22, 26
  };
  EXPECT_FILTERED_EQ(nums, expected, isEven);
}

TEST(TestIFilter, TestAllTrue) {
  std::vector<int> nums {2, 6, 8, 16, 22, 26};
  std::vector<int> expected {
    2, 6, 8, 16, 22, 26
  };
  EXPECT_FILTERED_EQ(nums, expected, isEven);
}

TEST(TestIFilter, TestNoneTrue) {
  std::vector<int> nums {1, 5, 7, 9, 17};
  std::vector<int> expected {};
  EXPECT_FILTERED_EQ(nums, expected, isEven);
}
