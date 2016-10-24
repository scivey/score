#include <gtest/gtest.h>
#include <string>
#include "score/nlp/stemming/Utf8Stemmer.h"
#include "score/nlp/Language.h"
using namespace score::nlp;
using namespace score::nlp::stemming;
using namespace std;

TEST(TestUf8Stemmer, TestGetStemPos) {
  string toStem = "testing";
  Utf8Stemmer stemmer(Language::EN);
  EXPECT_EQ(4, stemmer.getStemPos(toStem));
  EXPECT_EQ(4, stemmer.getStemPos(toStem.c_str(), toStem.size()));
}

TEST(TestUf8Stemmer, TestStemInPlace) {
  string toStem = "testing";
  Utf8Stemmer stemmer(Language::EN);
  stemmer.stemInPlace(toStem);
  EXPECT_EQ("test", toStem);
}

TEST(TestUf8Stemmer, TestStem) {
  string toStem = "testing";
  Utf8Stemmer stemmer(Language::EN);
  auto stemmed = stemmer.stem(toStem);
  EXPECT_EQ("test", stemmed);
  EXPECT_EQ("testing", toStem);
}

