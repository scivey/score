#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <unicode/brkiter.h>
#include "score_nlp/tokenize/ICUWordBreakView.h"

using namespace std;
using score::nlp::tokenize::ICUWordBreakView;
using score::nlp::Language;

TEST(TestICUWordBreakView, TestBasicEnglish) {
  string someStr = "this is a test";
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

TEST(TestICUWordBreakView, TestBasicGerman) {
  string someStr = "Hallo, wie geht's?";
  std::vector<int32_t> expected {
    0, 5, 6, 7, 10, 11, 17, 18
  };
  auto breakView = ICUWordBreakView::create(Language::DE);
  EXPECT_TRUE(breakView.valid());
  breakView.setText(someStr);
  std::vector<int32_t> actual;
  for (auto idx: breakView) {
    actual.push_back(idx);
  }
  EXPECT_EQ(expected, actual);
}
