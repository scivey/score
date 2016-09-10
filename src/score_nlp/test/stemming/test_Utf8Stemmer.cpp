#include <gtest/gtest.h>
#include <string>
#include "score_nlp/stemming/Utf8Stemmer.h"
#include "score_nlp/Language.h"
using namespace score::nlp;
using namespace score::nlp::stemming;
using namespace std;

TEST(TestUf8Stemmer, TestWorks) {
  string toStem = "testing";
  Utf8Stemmer stemmer(Language::EN);
  EXPECT_EQ(4, stemmer.getStemPos(toStem));
}
