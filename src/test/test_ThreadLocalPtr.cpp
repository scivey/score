#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include "aliens/ThreadLocalPtr.h"
#include "aliens/Counter.h"
#include "aliens/SingletonWrapper.h"
#include "aliens/test_support/Noisy.h"

using namespace std;

using Noisy = aliens::test_support::Noisy<17>;


TEST(TestLocalPtr, TestSimple) {
  auto rep = Noisy::getReport();
  rep->reset();
  EXPECT_EQ(0, rep->nCreated());
  {
    auto localPtr = aliens::ThreadLocalPtr<Noisy>::create();
    {
      EXPECT_EQ(0, rep->nCreated());
      auto x = localPtr.get();
      EXPECT_EQ(1, rep->nCreated());
      auto y = localPtr.get();
      EXPECT_EQ(x, y);
      EXPECT_EQ(1, rep->nCreated());
      std::thread t1([&localPtr]() {
        auto rep = Noisy::getReport();
        auto z1 = localPtr.get();
        EXPECT_EQ(2, rep->nCreated());
        auto z2 = localPtr.get();
        EXPECT_EQ(z1, z2);
        EXPECT_EQ(2, rep->nCreated());
        EXPECT_EQ(2, rep->nOutstanding());
        EXPECT_EQ(0, rep->nDestroyed());
      });
      t1.join();
      EXPECT_EQ(2, rep->nCreated());
      EXPECT_EQ(1, rep->nOutstanding());
      EXPECT_EQ(1, rep->nDestroyed());
    }
    {
      EXPECT_EQ(2, rep->nCreated());
      EXPECT_EQ(1, rep->nOutstanding());
      EXPECT_EQ(1, rep->nDestroyed());
      std::thread t1([&localPtr]() {
        auto rep = Noisy::getReport();
        auto z1 = localPtr.get();
        EXPECT_EQ(3, rep->nCreated());
        auto z2 = localPtr.get();
        EXPECT_EQ(z1, z2);
        EXPECT_EQ(3, rep->nCreated());
        EXPECT_EQ(2, rep->nOutstanding());
        EXPECT_EQ(1, rep->nDestroyed());
      });
      t1.join();
      EXPECT_EQ(3, rep->nCreated());
      EXPECT_EQ(1, rep->nOutstanding());
      EXPECT_EQ(2, rep->nDestroyed());
    }
    EXPECT_EQ(3, rep->nCreated());
    EXPECT_EQ(2, rep->nDestroyed());
    EXPECT_EQ(1, rep->nOutstanding());
  }
  EXPECT_EQ(3, rep->nCreated());
  EXPECT_EQ(1, rep->nOutstanding());
}
