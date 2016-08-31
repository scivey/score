#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "aliens/io/UTF8View.h"
#include "aliens/io/BytesView.h"

using namespace std;
using namespace aliens::io;

TEST(TestUTF8View, TestSimple) {
  string text = "this isn\u2019t a test, or is it?";
  vector<uint32_t> expectedPoints;
  {
    string firstPart = "isn";
    for (auto c: firstPart) {
      expectedPoints.push_back((uint32_t) c);
    }
    expectedPoints.push_back(8217);
    string secondPart = "t a test";
    for (auto c: secondPart) {
      expectedPoints.push_back((uint32_t) c);
    }
  }
  vector<uint32_t> actual;
  BytesView<char> window(text, 5, text.find(", or is it?"));
  for (auto c: window.asUTF8View()) {
    actual.push_back(c);
  }
  EXPECT_EQ(expectedPoints, actual);
}
