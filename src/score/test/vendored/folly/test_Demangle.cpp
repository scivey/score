#include <gtest/gtest.h>
#include "score/vendored/folly/Demangle.h"

using namespace std;
using score::vendored::folly::demangle;

class Something {};

TEST(TestDemangle, TestWorks) {
  auto name = demangle(typeid(Something));
  auto foundIdx = name.find("Something");
  EXPECT_TRUE(foundIdx != string::npos);
}
