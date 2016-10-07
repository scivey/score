#include <gtest/gtest.h>
#include "score/re/MultiMatcher.h"

using score::re::MultiMatcher;

TEST(TestMultiMatcher, TestSanity) {
  MultiMatcher::string_set_t words {
    "one", "two", "three"
  };
  auto matcher = MultiMatcher::fromWordSet(words);
  for (const auto& word: words) {
    EXPECT_TRUE(matcher->matches(word));
  }
  EXPECT_FALSE(matcher->matches("dog"));
  EXPECT_FALSE(matcher->matches("on"));
  EXPECT_FALSE(matcher->matches("ree"));
  EXPECT_FALSE(matcher->matches("hre"));
  EXPECT_FALSE(matcher->matches("thr"));
  EXPECT_FALSE(matcher->matches("cat"));
}
