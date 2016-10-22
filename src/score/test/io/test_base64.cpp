#include <gtest/gtest.h>
#include <string>
#include "score/io/base64.h"

using namespace std;
using score::io::base64;

TEST(Test_base64, TestEncode) {
  string input = "fish";
  auto encoded = base64::encode(input);
  EXPECT_EQ("ZmlzaA==", encoded);
}

TEST(Test_base64, TestDecode) {
  string input = "ZmlzaA==";
  auto decoded = base64::decode(input);
  EXPECT_EQ("fish", decoded);
}
