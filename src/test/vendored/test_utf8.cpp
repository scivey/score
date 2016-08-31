#include <gtest/gtest.h>
#include <string>

#include "score/vendored/utf8/utf8.h"
using namespace std;
namespace utf8 = score::vendored::utf8;


TEST(TestUtf8, TestNotNonsensicalAscii) {
  string msg {"TEST"};
  std::vector<uint32_t> expected;
  for (auto c: msg) {
    auto c8 = (uint8_t) c;
    expected.push_back(c8);
  }
  std::vector<uint32_t> actual;
  auto it = msg.begin();
  while (it != msg.end()) {
    uint32_t current = utf8::unchecked::next(it);
    actual.push_back(current);
  }
  EXPECT_EQ(expected, actual);
}
