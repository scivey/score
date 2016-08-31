#include <gtest/gtest.h>
#include <functional>
#include <atomic>
#include <memory>
#include <string>
#include "score/NullablePointer.h"

using namespace std;
using score::NullablePointer;

TEST(TestNullablePointer, TestSanity) {
  NullablePointer<string> stringPtr;
  EXPECT_FALSE(stringPtr.hasValue());
  auto aString = std::make_shared<string>("xyz");
  stringPtr.assign(aString.get());
  EXPECT_TRUE(stringPtr.hasValue());
  string result1 = *stringPtr;
  EXPECT_EQ("xyz", result1);
  EXPECT_TRUE(stringPtr.hasValue());
  string* rawPtr = stringPtr.get();
  EXPECT_EQ(3, rawPtr->size());
  EXPECT_EQ(3, stringPtr->size());
  string result2 = *rawPtr;
  EXPECT_EQ("xyz", result2);
  EXPECT_TRUE(stringPtr.hasValue());
  stringPtr.reset();
  EXPECT_FALSE(stringPtr.hasValue());
}

TEST(TestNullablePointer, TestReference) {
  NullablePointer<string> stringPtr;
  auto aString = std::make_shared<string>("abc");
  stringPtr.assign(aString.get());
  string result = stringPtr.value();
  EXPECT_EQ("abc", result);
}

TEST(TestNullablePointer, TestNullConstruction) {
  NullablePointer<string> stringPtr;
  EXPECT_FALSE(stringPtr.hasValue());
  EXPECT_FALSE(!!stringPtr);
}

TEST(TestNullablePointer, TestNonNullConstruction) {
  auto aString = std::make_shared<string>("hij");
  NullablePointer<string> stringPtr(aString.get());
  EXPECT_TRUE(stringPtr.hasValue());
  EXPECT_TRUE(!!stringPtr);
}
