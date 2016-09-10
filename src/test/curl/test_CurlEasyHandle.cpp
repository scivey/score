#include <gtest/gtest.h>
#include "score/curl/CurlEasyHandle.h"
#include "score/curl/RealCurlAPI.h"


TEST(TestCurlEasyHandle, TestSanity) {
  auto handle = CurlEasyHandle::create();
  handle.setUrl("http://www.something.com");
  EXPECT_EQ("http://www.something.com", handle.getUrl());
}
