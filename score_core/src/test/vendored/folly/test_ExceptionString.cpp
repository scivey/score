#include <gtest/gtest.h>
#include <stdexcept>
#include "score/vendored/folly/ExceptionString.h"

using namespace std;
using score::vendored::folly::exceptionStr;

class SomeException : public std::runtime_error {
 public:
  SomeException(const std::string &msg): std::runtime_error(msg){}
};

TEST(TestExceptionString, TestWorks) {
  SomeException err {"zebras"};
  auto asString = exceptionStr(err);
  EXPECT_TRUE(asString.find("SomeException") != string::npos);
  EXPECT_TRUE(asString.find("zebras") != string::npos);
}
