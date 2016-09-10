#include <gtest/gtest.h>
#include "score/curl/detail/CurlEasyHandle.h"
#include "score/curl/detail/RealCurlAPI.h"

using namespace score::curl::detail;
using namespace score::curl;

using RealEasyHandle = CurlEasyHandle<RealCurlAPI>;

TEST(TestCurlEasyHandle, TestSanity) {
  auto handle = RealEasyHandle::create();
  handle.setUrl("http://www.something.com");
  EXPECT_EQ("http://www.something.com", handle.getUrl());
}
