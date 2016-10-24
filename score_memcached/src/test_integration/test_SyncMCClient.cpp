#include <gtest/gtest.h>
#include "score_memcached/SyncMCClient.h"
#include "score_memcached/MemcachedConfig.h"

using namespace score::memcached;
using namespace std;

#define CHECK_SET(client, key, val) \
  do { \
    EXPECT_TRUE(client.isConnected()); \
    auto setResponse = client.set(key, val); \
    EXPECT_FALSE(setResponse.hasException()); \
  } while (0)

#define EXPECT_TO_GET(client, key, expected) \
  do { \
    EXPECT_TRUE(client.isConnected()); \
    auto getResponse = client.get(key); \
    EXPECT_FALSE(getResponse.hasException()); \
    std::string actual = getResponse.value().value(); \
    EXPECT_EQ(expected, actual); \
  } while (0)


TEST(IntegrationTestSyncMCClient, TestSanity1) {
  SyncMCClient client { MemcachedConfig {
    {"127.0.0.1", 11211}
  }};
  client.connectExcept();
  EXPECT_TRUE(client.isConnected());
  CHECK_SET(client, "foo", "f1");
  EXPECT_TO_GET(client, "foo", "f1");
  CHECK_SET(client, "bar", "b1");
  EXPECT_TO_GET(client, "bar", "b1");
  CHECK_SET(client, "foo", "f2");
  EXPECT_TO_GET(client, "foo", "f2");
  EXPECT_TO_GET(client, "bar", "b1");
  EXPECT_TO_GET(client, "foo", "f2");
}
