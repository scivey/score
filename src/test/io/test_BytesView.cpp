#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "score/io/BytesView.h"

using namespace std;

using BytesView = score::io::BytesView<char>;

TEST(TestBytesView, EntireString) {
  string text = "this is a test";
  string expected = "this is a test";
  string actual = "";
  BytesView window(text);
  for (auto c: window) {
    actual += c;
  }
  EXPECT_EQ(expected, actual);
}

TEST(TestBytesView, EntireStringFromPointers) {
  string text = "this is a test";
  string expected = "this is a test";
  string actual = "";
  BytesView window(text.data(), text.data() + text.size());
  for (auto c: window) {
    actual += c;
  }
  EXPECT_EQ(expected, actual);
}

TEST(TestBytesView, EntireStringFromPointerAndSize) {
  string text = "this is a test";
  string expected = "this is a test";
  string actual = "";
  BytesView window(text.data(), text.size());
  for (auto c: window) {
    actual += c;
  }
  EXPECT_EQ(expected, actual);
}


TEST(TestBytesView, HalfString1) {
  string text = "this is a test";
  string expected = "this is";
  string actual = "";
  BytesView window(text, 0, 7);
  for (auto c: window) {
    actual += c;
  }
  EXPECT_EQ(expected, actual);
}

TEST(TestBytesView, HalfString2) {
  string text = "this is a test";
  string expected = "a test";
  string actual = "";
  BytesView window(text, 8, text.size());
  for (auto c: window) {
    actual += c;
  }
  EXPECT_EQ(expected, actual);
}


TEST(TestBytesView, MidString) {
  string text = "this is a test";
  string expected = "is a";
  string actual = "";
  BytesView window(text, 5, 9);
  for (auto c: window) {
    actual += c;
  }
  EXPECT_EQ(expected, actual);
}


