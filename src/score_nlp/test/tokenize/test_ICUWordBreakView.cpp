#include <gtest/gtest.h>
#include <unicode/brkiter.h>
#include "score_nlp/tokenize/ICUWordBreakView.h"

using score::nlp::tokenize::ICUWordBreakView;
using score::nlp::Language;

TEST(TestICUWordBreakView, TestBasicEnglish) {
  UnicodeString someStr = "this is a test";
  std::vector<int32_t> expected {
    0, 4, 5, 7, 8, 9, 10, 14
  };
  auto breakView = ICUWordBreakView::create(Language::EN);
  EXPECT_TRUE(breakView.valid());
  breakView.setText(someStr);
  std::vector<int32_t> actual;
  for (auto idx: breakView) {
    actual.push_back(idx);
  }
  EXPECT_EQ(expected, actual);
}
