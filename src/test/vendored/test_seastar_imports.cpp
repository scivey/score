#include <gtest/gtest.h>
#include <bitset>
#include <vector>
#include "aliens/vendored/seastar/core/bitset-iter.hh"
/*
  These are just quick sanity checks, not exhaustive tests,
  since seastar has its own test suite.
*/

using namespace std;
namespace bitsets = aliens::vendored::seastar::bitsets;

TEST(TestSeaStarImports, TestBitsetIter) {
  std::bitset<64> someBits {0};
  someBits.set(5);
  someBits.set(7);
  someBits.set(38);
  std::vector<size_t> indices;
  for (auto idx: bitsets::for_each_set(someBits)) {
    indices.push_back(idx);
  }
  std::vector<size_t> expected {
    5, 7, 38
  };
  EXPECT_EQ(expected, indices);
}
