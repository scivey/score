#include <gtest/gtest.h>
#include <mutex>
#include <thread>

#include "asioz/Counter.h"
#include "asioz/SingletonWrapper.h"

using asioz::Counter;
using asioz::SingletonWrapper;
using StaticCounter = SingletonWrapper<Counter<29>>;

TEST(TestSingletonWrapper, SimpleTest) {
  StaticCounter::get()->reset();
  size_t n = 9999;
  {
    n = StaticCounter::get()->get();
    EXPECT_EQ(0, n);
    n = StaticCounter::get()->get();
    EXPECT_EQ(1, n);
  }
  {
    n = StaticCounter::get()->get();
    EXPECT_EQ(2, n);
    n = StaticCounter::get()->get();
    EXPECT_EQ(3, n);
  }
  {
    EXPECT_EQ(4, StaticCounter::get()->get());
    StaticCounter::get()->reset();
    EXPECT_EQ(0, StaticCounter::get()->get());
  }
  {
    EXPECT_EQ(1, StaticCounter::get()->get());
    EXPECT_EQ(2, StaticCounter::get()->get());
    EXPECT_EQ(3, StaticCounter::get()->get());
  }
  EXPECT_EQ(4, StaticCounter::get()->get());
  {
    EXPECT_EQ(5, StaticCounter::get()->get());
  }
  {
    StaticCounter::get()->reset();
  }
  EXPECT_EQ(0, StaticCounter::get()->get());
}
