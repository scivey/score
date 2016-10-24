#include <gtest/gtest.h>
#include "score_bench/bench.h"

using namespace score;
using namespace score::bench;
using namespace std;


TEST(TestBench, TestRunBenched) {
  size_t counter {0};
  auto work = [&counter]() {
    counter++;
  };
  auto result = score::bench::runBenched("something", work, 12);
  EXPECT_EQ(12, counter);
  EXPECT_EQ("something", result.getName());
}

