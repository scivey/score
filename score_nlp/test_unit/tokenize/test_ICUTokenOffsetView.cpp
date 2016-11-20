#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <unicode/brkiter.h>
#include "score/nlp/tokenize/ICUBreakView.h"
#include "score/nlp/tokenize/ICUTokenOffsetView.h"

using namespace std;
using score::nlp::tokenize::ICUBreakView;
using score::nlp::tokenize::ICUTokenOffsetView;
using score::nlp::Language;

TEST(TestICUTokenOffsetView, TestBasicEnglishAscii) {
  string someStr = "this is a test";
  std::vector<std::pair<int32_t, int32_t>> expected {
    {0, 4},
    {4, 5},
    {5, 7},
    {7, 8},
    {8, 9},
    {9, 10},
    {10, 14}
  };
  auto tokenView = ICUTokenOffsetView::create(Language::EN);
  tokenView.setText(someStr);
  std::vector<pair<int32_t, int32_t>> actual;
  for (auto idx: tokenView) {
    actual.push_back(idx);
  }
  EXPECT_EQ(expected, actual);
}

TEST(TestICUTokenOffsetView, TestBasicEnglishUnicode) {
  string someStr = "this is \u2019 test";
  std::vector<std::pair<int32_t, int32_t>> expected {
    {0, 4},
    {4, 5},
    {5, 7},
    {7, 8},
    {8, 11},
    {11, 12},
    {12, 16}
  };
  auto tokenView = ICUTokenOffsetView::create(Language::EN);
  tokenView.setText(someStr);
  std::vector<pair<int32_t, int32_t>> actual;
  for (auto idx: tokenView) {
    actual.push_back(idx);
  }
  EXPECT_EQ(expected, actual);
}
