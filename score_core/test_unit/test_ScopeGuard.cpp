#include <gtest/gtest.h>
#include <functional>
#include <atomic>
#include "score/ScopeGuard.h"

using namespace std;
using score::ScopeGuard;

TEST(TestScopeGuard, TestWorks) {
  int x = 0;
  {
    ScopeGuard guard([&x]() {
       x = 17;
    });
    EXPECT_EQ(0, x);
  }
  EXPECT_EQ(17, x);
  x = 0;
  {
    ScopeGuard guard([&x]() {
      x = 26;
    });
    EXPECT_EQ(0, x);
    guard.dismiss();
    EXPECT_EQ(0, x);
  }
  EXPECT_EQ(0, x);
}
