#include <gtest/gtest.h>
#include <bitset>
#include <vector>
#include "aliens/vendored/folly/FBVector.h"

/*
  These are just quick sanity checks, not exhaustive tests,
  since folly has its own test suite.
*/

using namespace std;
namespace folly = aliens::vendored::folly;

TEST(TestFollyImports, TestFBVector) {
  fbvector<int> instance;
  EXPECT_TRUE(true);
}
