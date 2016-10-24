#include <gtest/gtest.h>
#include <string>
#include "score/nlp/UTF8UTextRef.h"

using namespace std;
using score::nlp::UTF8UTextRef;

TEST(TestUTF8UTextRef, TestConstruction1) {
  string something = "fish";
  auto ref = UTF8UTextRef::fromUTF8(something);
  EXPECT_EQ(4, ref.size());
  EXPECT_TRUE(ref.valid());
}

TEST(TestUTF8UTextRef, TestConstruction2) {
  string something = "fish";
  auto ref = UTF8UTextRef::fromUTF8(something);
  EXPECT_TRUE(ref.valid());
  UTF8UTextRef ref2 {std::move(ref)};
  EXPECT_EQ(4, ref2.size());
  EXPECT_TRUE(ref2.valid());
  EXPECT_FALSE(ref.valid());
}

TEST(TestUTF8UTextRef, TestAssignment) {
  string something = "fish";
  auto ref1 = UTF8UTextRef::fromUTF8(something);
  EXPECT_EQ(4, ref1.size());
  EXPECT_TRUE(ref1.valid());
  UTF8UTextRef ref2;
  EXPECT_FALSE(ref2.valid());
  ref2 = std::move(ref1);
  EXPECT_FALSE(ref1.valid());
  EXPECT_TRUE(ref2.valid());
  EXPECT_EQ(4, ref2.size());
}
