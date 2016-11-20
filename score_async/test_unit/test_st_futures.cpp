#include "score/async/futures/st/STFuture.h"
#include "score/async/futures/st/STPromise.h"
#include "score/async/futures/st/helpers.h"
#include "score/mem/st_shared_ptr.h"

#include <gtest/gtest.h>

namespace mem = score::mem;
namespace st = score::async::futures::st;


TEST(Test_st_futures, TestSanity1) {
  auto prom = mem::make_st_shared<st::STPromise<int>>();
  int calledWith {0};
  auto later = prom->getFuture();
  later.then([&calledWith](int x) {
    calledWith = x;
  });
  prom->setValue(28);
  EXPECT_EQ(28, calledWith);
}

TEST(Test_st_futures, TestSanity2) {
  auto prom = mem::make_st_shared<st::STPromise<int>>();
  std::vector<int> results;
  auto later = prom->getFuture();
  later.then([&results](int x) {
    results.push_back(x);
    return x * 10;
  }).then([&results](int x) {
    results.push_back(x);
    return x * 10;
  }).then([&results](int x) {
    results.push_back(x);
  });
  prom->setValue(31);
  std::vector<int> expected {31, 310, 3100};
  EXPECT_EQ(expected, results);
}

TEST(Test_st_futures, TestUnwrap) {
  auto prom = mem::make_st_shared<st::STPromise<int>>();
  std::vector<int> results;
  auto later = prom->getFuture();
  later.then([&results](int x) {
    results.push_back(x);
    return x * 10;
  }).then([&results](int x) {
    results.push_back(x);
    return st::makeReadySTFuture<int>(26);
  }).then([&results](int x) {
    results.push_back(x);
  });
  prom->setValue(31);
  std::vector<int> expected {31, 310, 26};
  EXPECT_EQ(expected, results);
}











