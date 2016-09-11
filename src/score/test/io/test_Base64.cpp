#include <gtest/gtest.h>
#include <string>
#include "score/io/Base64.h"

using namespace std;
using score::io::Base64;

TEST(TestBase64, TestEncode) {
  string input = "fish";
  auto encoded = Base64::encode(input);
  EXPECT_EQ("ZmlzaA==", encoded);
}

TEST(TestBase64, TestDecode) {
  string input = "ZmlzaA==";
  auto decoded = Base64::decode(input);
  EXPECT_EQ("fish", decoded);
}
