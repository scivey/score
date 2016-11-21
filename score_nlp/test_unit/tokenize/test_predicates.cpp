#include <string>
#include <vector>

#include <gtest/gtest.h>
#include "score/nlp/tokenize/predicates.h"

using namespace std;
using namespace score::nlp::tokenize;

TEST(TestTokenizePredicates, TestIsWordToken) {
  EXPECT_TRUE(isWordToken(string{"yes"}));
  EXPECT_FALSE(isWordToken(string{" "}));
  EXPECT_TRUE(isWordToken(string{"don't"}));
  EXPECT_FALSE(isWordToken(string{"'t"}));
}

