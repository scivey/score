#include "score/locks/PThreadSpinLock.h"
#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include <exception>
#include <stdexcept>
#include <vector>
#include <mutex>

using score::locks::PThreadSpinLock;
using namespace std;

TEST(TestPThreadSpinLock, TestSanity) {
  auto slock = PThreadSpinLock::create().value();
  EXPECT_TRUE(slock.try_lock());
  EXPECT_FALSE(slock.try_lock());
  slock.unlock();
  EXPECT_TRUE(slock.try_lock());
  EXPECT_FALSE(slock.try_lock());
  slock.unlock();
}

template<template<class...> class TGuardTmpl>
void testGuardType() {
  auto slock = PThreadSpinLock::create().value();
  {
    TGuardTmpl<PThreadSpinLock> guard {slock};
    EXPECT_FALSE(slock.try_lock());
  }
  EXPECT_TRUE(slock.try_lock());
  slock.unlock();
}

TEST(TestPThreadSpinLock, TestLockGuardCompatibility1) {
  testGuardType<std::lock_guard>();
}

TEST(TestPThreadSpinLock, TestLockGuardCompatibility2) {
  testGuardType<std::unique_lock>();
}

