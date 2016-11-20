#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <unicode/brkiter.h>
#include "score/nlp/tokenize/ICUBreakView.h"

using namespace std;
using score::nlp::tokenize::ICUBreakView;
using score::nlp::Language;

TEST(TestICUBreakView, TestBasicEnglish) {
  string someStr = "this is a test";
  std::vector<int32_t> expected {
    0, 4, 5, 7, 8, 9, 10, 14
  };
  auto breakView = ICUBreakView::create(Language::EN);
  EXPECT_TRUE(breakView.valid());
  breakView.setText(someStr);
  std::vector<int32_t> actual;
  for (auto idx: breakView) {
    actual.push_back(idx);
  }
  EXPECT_EQ(expected, actual);
}

TEST(TestICUBreakView, TestBasicGerman) {
  string someStr = "Hallo, wie geht's?";
  std::vector<int32_t> expected {
    0, 5, 6, 7, 10, 11, 17, 18
  };
  auto breakView = ICUBreakView::create(Language::DE);
  EXPECT_TRUE(breakView.valid());
  breakView.setText(someStr);
  std::vector<int32_t> actual;
  for (auto idx: breakView) {
    actual.push_back(idx);
  }
  EXPECT_EQ(expected, actual);
}
