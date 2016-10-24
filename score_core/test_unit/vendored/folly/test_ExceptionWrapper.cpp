#include "score/vendored/folly/ExceptionWrapper.h"
#include <gtest/gtest.h>
#include <stdexcept>

using namespace std;
using score::vendored::folly::make_exception_wrapper;

class SomeException : public std::runtime_error {
 public:
  SomeException(const string &msg): std::runtime_error(msg){}
};

TEST(TestExceptionWrapper, TestSanity) {
  auto wrapped = make_exception_wrapper<SomeException>("876");
  EXPECT_TRUE(wrapped.what().find("876") != string::npos);
}


class Err1: public std::runtime_error {
 public:
  Err1(const string &msg): std::runtime_error(msg){}
};

class Err2: public std::runtime_error {
 public:
  Err2(const string &msg): std::runtime_error(msg){}
};

TEST(TestExceptionWrapper, TestWithException1) {
  auto wrapped = make_exception_wrapper<Err1>("xx");
  bool toSet = false;
  bool called = wrapped.with_exception<Err1>([&toSet](const Err1&) {
    toSet = true;
  });
  EXPECT_TRUE(called);
  EXPECT_TRUE(toSet);
}

TEST(TestExceptionWrapper, TestWithException2) {
  auto wrapped = make_exception_wrapper<Err1>("xx");
  bool toSet = false;
  bool called = wrapped.with_exception<Err2>([&toSet](const Err2&) {
    toSet = true;
  });
  EXPECT_FALSE(called);
  EXPECT_FALSE(toSet);
}