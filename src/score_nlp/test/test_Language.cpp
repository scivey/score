#include <gtest/gtest.h>
#include <string>
#include "score_nlp/Language.h"

using namespace score::nlp;
using namespace std;

TEST(TestLanguage, TestDetection1) {
  string sample = "this is a text";
  string name = detectLanguageName(sample);
  EXPECT_EQ("ENGLISH", name);
}

TEST(TestLanguage, TestDetection2) {
  string sample = "no me gusta la biblioteca";
  string name = detectLanguageName(sample);
  EXPECT_EQ("SPANISH", name);
}
