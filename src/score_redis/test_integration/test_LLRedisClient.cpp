#include <gtest/gtest.h>
#include "score_redis/LLRedisClient.h"
#include "score_redis/RedisDynamicResponse.h"
#include "score_async/EventContext.h"

using namespace score::redis;
using namespace std;
using ResponseType = RedisDynamicResponse::ResponseType;

#define EXPECT_INT_RESPONSE(responseOpt, n) \
  do { \
    EXPECT_TRUE(responseOpt.value().isType(ResponseType::INTEGER)); \
    EXPECT_EQ(n, responseOpt.value().getInt().value()); \
  } while (0)

#define EXPECT_STRING_RESPONSE(responseOpt, aStr) \
  do { \
    EXPECT_TRUE(responseOpt.value().isType(ResponseType::STRING)); \
    EXPECT_EQ(std::string {aStr}, responseOpt.value().getString().value()); \
  } while (0)


#define EXPECT_STATUS(responseOpt) \
  do { \
    EXPECT_TRUE(responseOpt.value().isType(ResponseType::STATUS)); \
  } while (0);

TEST(IntegrationTestLLRedisClient, TestSanity1) {
  auto evtx = EventContext::
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
