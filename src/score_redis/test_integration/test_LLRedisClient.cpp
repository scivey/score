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
  std::shared_ptr<LLRedisClient> makeClient() {
    return util::createShared<LLRedisClient>(
      eventContext.get(), score::io::SocketAddr{"127.0.0.1", 6379}
    );
  }
  static std::shared_ptr<LLRTestContext> createShared() {
    auto ctx = std::make_shared<LLRTestContext>();
    ctx->eventContext = util::createShared<EventContext>();
    ctx->client = ctx->makeClient();
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


struct SubMessage {
  using string_t = std::string;
  string_t channelName;
  string_t messageBody;
  SubMessage(string_t&& name, string_t&& body)
    : channelName(std::forward<string_t>(name)),
      messageBody(std::forward<string_t>(body)) {}
  SubMessage(RedisDynamicResponse& nameResponse, RedisDynamicResponse& bodyResponse) {
    EXPECT_TRUE(nameResponse.isType(ResponseType::STRING));
    channelName = nameResponse.getString().value();
    EXPECT_TRUE(bodyResponse.isType(ResponseType::STRING) || bodyResponse.isType(ResponseType::INTEGER));
    if (bodyResponse.isType(ResponseType::STRING)) {
      messageBody = bodyResponse.getString().value();
    } else {
      std::ostringstream oss;
      oss << bodyResponse.getInt().value();
      messageBody = oss.str();
    }
  }
};

class CallbackSubEventHandler: public LLRedisSubscription::EventHandler {
 public:
  using cb_t = Function<void, RedisDynamicResponse>;
 protected:
  cb_t callback_;
 public:
  CallbackSubEventHandler(cb_t&& callback): callback_(std::forward<cb_t>(callback)){}
  void onMessage(RedisDynamicResponse&& res) override {
    callback_(std::forward<RedisDynamicResponse>(res));
  }
  void onStarted() override {
    LOG(INFO) << "onStart!";
  }
  void onStopped() override {
    LOG(INFO) << "onStop!";
  }
};

TEST(IntegrationTestLLRedisClient, TestSubscription1) {
  auto ctx = LLRTestContext::createShared();
  using cb_t = typename CallbackSubEventHandler::cb_t;
  const size_t kTotalMessages = 10;
  const string kChannelName = "some-channel";

  std::vector<string> toSend;
  {
    string alphabet = "abcdefghijklmnopqrstuvwxyz";
    for (auto c: alphabet) {
      ostringstream msg;
      msg << "msg-" << c;
      toSend.push_back(msg.str());
      if (toSend.size() >= kTotalMessages) {
        break;
      }
    }
  }

  std::shared_ptr<CallbackSubEventHandler> eventHandler {nullptr};
  std::vector<SubMessage> received;
  bool onFinishedCalled {false};
  auto onFinished = [&eventHandler, &received, &toSend, ctx, kChannelName, &onFinishedCalled]() {
    EXPECT_TRUE(!!eventHandler);
    eventHandler->stop();
    std::vector<std::string> messageBodies;
    std::vector<std::string> actual = toSend;
    for (auto& msg: received) {
      EXPECT_EQ(kChannelName, msg.channelName);
      messageBodies.push_back(msg.messageBody);
    }
    std::sort(messageBodies.begin(), messageBodies.end());
    std::sort(actual.begin(), actual.end());
    EXPECT_EQ(actual, messageBodies);
    onFinishedCalled = true;
    ctx->finished();
  };

  cb_t callback{[kTotalMessages, &received, &onFinished](RedisDynamicResponse&& res) {
    EXPECT_TRUE(res.isType(ResponseType::ARRAY));
    auto asArray = res.getArray().value();
    EXPECT_EQ(3, asArray.size());
    auto msgType = asArray[0].getString().value();
    if (msgType == "message") {
      received.push_back(SubMessage{asArray[1], asArray[2]});
    }
    if (received.size() >= kTotalMessages) {
      onFinished();
    }
  }};

  std::shared_ptr<LLRedisClient> publisher {nullptr};
  auto runPublisher = [ctx, kChannelName, &toSend, &publisher]() {
    auto pubClient = ctx->makeClient();
    publisher = pubClient;
    pubClient->connect().then([pubClient, kChannelName, &toSend](score::Try<score::Unit> outcome) {
      outcome.throwIfFailed();
      for (auto& msg: toSend) {
        pubClient->publish(kChannelName, msg, [pubClient](res_t&& response) {
          response.throwIfFailed();
        });
      }
    });
  };
  std::shared_ptr<LLRedisSubscription> subPtr {nullptr};
  eventHandler = std::make_shared<CallbackSubEventHandler>(std::move(callback));
  ctx->connect([ctx, &subPtr, eventHandler, &runPublisher]() {
    auto client = ctx->client;
    auto trySub = client->subscribe("some-channel", eventHandler);
    trySub.throwIfFailed();
    subPtr = trySub.value();
    runPublisher();
  });
  ctx->run();
  EXPECT_TRUE(onFinishedCalled);
}
