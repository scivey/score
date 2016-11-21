#include <gtest/gtest.h>
#include <string>
#include "score/nlp/Language.h"

using namespace score::nlp;
using namespace std;

TEST(TestLanguage, TestShortCodeConversion) {
  EXPECT_EQ(Language::EN, languageFromCode("en"));
  EXPECT_EQ(Language::DE, languageFromCode("de"));
}
