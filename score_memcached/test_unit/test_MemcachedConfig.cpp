#include <gtest/gtest.h>
#include <sstream>
#include <memory>
#include <string>
#include "score/memcached/MemcachedConfig.h"

using namespace score::memcached;
using namespace std;

TEST(TestMemcachedConfig, TestToStringValid) {
  MemcachedConfig config {
    {"127.0.0.1", 11211}
  };
  auto configStr = config.toConfigString();
  EXPECT_FALSE(configStr.hasException());
  EXPECT_TRUE(configStr.value().find("127.0.0.1:11211") != string::npos);
}

TEST(TestMemcachedConfig, TestToStringInvalid) {
  MemcachedConfig config {
    {"9999", 11211}
  };
  auto configStr = config.toConfigString();
  EXPECT_TRUE(configStr.hasException());
}

