#include <gtest/gtest.h>
#include <bitset>
#include <vector>
#include <string>
#include "score/vendored/smhasher/City.h"
#include "score/vendored/smhasher/MurmurHash3.h"

/*
  This is just a quick sanity check to make sure
  smhasher integrates with the build - not a test
  of the hash functions.
*/

using namespace score::vendored::smhasher;
using namespace std;

TEST(TestSmhasherImports, TestCity) {
  string one {"one"}, two {"two"};
  auto hash1 = CityHash64(one.data(), one.size());
  auto hash2 = CityHash64(two.data(), two.size());
  EXPECT_NE(hash1, hash2);
}
