#include <gtest/gtest.h>
#include <vector>
#include <string>

#include "score/io/UTF8IndexView.h"
#include "score/io/UTF8IndexIterator.h"

using namespace std;
using score::io::UTF8IndexIterator;
using score::io::UTF8IndexView;

TEST(TestUTF8IndexView, SimpleAscii) {
  string text = "cat-related text";
  vector<pair<size_t, uint32_t>> expected;
  expected.reserve(text.size());
  for (size_t i = 0; i < text.size(); i++) {
    expected.push_back(make_pair(i, (uint32_t) text[i]));
  }
  UTF8IndexView view(text);
  vector<pair<size_t, uint32_t>> actual;
  actual.reserve(text.size());
  for (auto cpPair: view) {
    actual.push_back(cpPair);
  }
  EXPECT_EQ(expected, actual);
}

TEST(TestUTF8IndexView, SomeUnicode) {
  string text = "cat\u2014related text";
  vector<pair<size_t, uint32_t>> expected {
    {0, 99}, {1, 97}, {2, 116}, {3, 8212},
    {6, 114}, {7, 101}, {8, 108}, {9, 97},
    {10, 116}, {11, 101}, {12, 100}, {13, 32},
    {14, 116}, {15, 101}, {16, 120}, {17, 116}
  };
  UTF8IndexView view(text);
  vector<pair<size_t, uint32_t>> actual;
  actual.reserve(text.size());
  for (auto cpPair: view) {
    actual.push_back(cpPair);
  }
  EXPECT_EQ(expected, actual);
}

