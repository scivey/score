#include <gtest/gtest.h>
#include "score/test_support/Noisy.h"
#include "score/mem/LSharedPtr.h"
using namespace score::test_support;
using namespace score::mem;
using Noise = Noisy<59515>;

TEST(TestSeastarSharedPtr, PreTest) {
  Noise::getReport()->reset();
  EXPECT_EQ(0, Noise::getReport()->nCreated());
  Noise instance;
  EXPECT_EQ(1, Noise::getReport()->nCreated());
}

TEST(TestSeastarSharedPtr, TestSanity) {
  Noise::getReport()->reset();
  {
    Noise n1;
    Noise n2;
    auto i1 = makeLShared(std::move(n1));
    auto i2 = makeLShared(std::move(n2));
  }
  EXPECT_EQ(2, Noise::getReport()->nCreated());
  EXPECT_EQ(2, Noise::getReport()->nDestroyed());
}

