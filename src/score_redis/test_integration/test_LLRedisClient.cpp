#include <gtest/gtest.h>
#include "score/io/SocketAddr.h"
#include "score/Try.h"
#include "score/Unit.h"
#include "score/func/Function.h"
#include "score_redis/LLRedisClient.h"
#include "score_redis/RedisDynamicResponse.h"
#include "score_async/EventContext.h"
#include "score/util/misc.h"

using score::Try;
using score::Unit;
using score::func::Function;
using namespace score::redis;
namespace util = score::util;
using score::async::EventContext;
using ResponseType = RedisDynamicResponse::ResponseType;
using namespace std;

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

using res_t = typename LLRedisClient::response_t;


#define EXPECT_STRING_ARRAY(responseOpt, ...) do { \
    std::vector<std::string> expStr { \
      __VA_ARGS__ \
    }; \
    std::vector<std::string> actual; \
    auto resVal = responseOpt.value(); \
    EXPECT_TRUE(resVal.isType(ResponseType::ARRAY)); \
    auto children = resVal.getArray().value(); \
    for (auto& child: children) { \
      EXPECT_TRUE(child.isType(ResponseType::STRING)); \
      actual.push_back(child.getString().value()); \
    } \
    EXPECT_EQ(expStr, actual); \
  } while (0)



struct LLRTestContext {
  std::shared_ptr<EventContext> eventContext {nullptr};
  std::atomic<bool> doneToken {false};
  std::shared_ptr<LLRedisClient> client {nullptr};
  using run_func_t = Function<void>;
  run_func_t runFunc;
  static std::shared_ptr<LLRTestContext> createShared() {
    auto ctx = std::make_shared<LLRTestContext>();
    ctx->eventContext = util::createShared<EventContext>();
    ctx->client = util::createShared<LLRedisClient>(
      ctx->eventContext.get(), score::io::SocketAddr{"127.0.0.1", 6379}
    );
    return ctx;
  }
  void connect(run_func_t&& func) {
    runFunc = std::forward<run_func_t>(func);
    client->connect().then([this](score::Try<score::Unit> result) {
      result.throwIfFailed();
      this->runFunc();
    });
  }
  void run() {
    while (!doneToken.load(std::memory_order_relaxed)) {
      eventContext->getBase()->runOnce();
    }
  }
  void finished() {
    client->disconnect().then([this](score::Try<score::Unit> result) {
      result.throwIfFailed();
      doneToken.store(true);
    });
  }
};

TEST(IntegrationTestLLRedisClient, TestGetSet) {
  auto ctx = LLRTestContext::createShared();
  ctx->connect([ctx]() {
    auto client = ctx->client;
    client->set("bar", "bar-value", [client, ctx](res_t response) {
      EXPECT_STATUS(response);
      client->get("bar", [client, ctx](res_t response) {
        EXPECT_STRING_RESPONSE(response, "bar-value");
        ctx->finished();
      });
    });
  });
  ctx->run();
}

TEST(IntegrationTestLLRedisClient, TestMgetMset) {
  auto ctx = LLRTestContext::createShared();
  ctx->connect([ctx]() {
    auto client = ctx->client;
    client->mset({{"a", "a-val"}, {"b", "b-val"}}, [client, ctx](res_t response) {
      EXPECT_STATUS(response);
      client->mget({"a", "b"}, [client, ctx](res_t response) {
        EXPECT_STRING_ARRAY(response, "a-val", "b-val");
        ctx->finished();
      });
    });
  });
  ctx->run();
}
