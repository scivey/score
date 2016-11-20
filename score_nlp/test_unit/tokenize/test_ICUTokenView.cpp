#include <string>
#include <vector>

#include <gtest/gtest.h>
#include "score/nlp/tokenize/ICUTokenView.h"

using namespace std;
using score::nlp::tokenize::ICUTokenView;
using score::nlp::Language;

TEST(TestICUTokenView, TestBasicEnglishAscii) {
  string someStr = "this is a test";
  std::vector<string> expected {
    "this", " ", "is", " ", "a", " ", "test"
  };
  auto tokenView = ICUTokenView::create(Language::EN);
  tokenView.setText(someStr);
  std::vector<string> actual;
  for (auto token: tokenView) {
    EXPECT_TRUE(token.valid());
    actual.push_back(token.copyToStdString());
  }
  EXPECT_EQ(expected, actual);
}
