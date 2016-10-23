#include <glog/logging.h>
#include <string>
#include <sstream>
#include <memory>
#include "score/func/Function.h"
#include "score/func/callable_traits.h"
#include "score_async/EventContext.h"
#include "score/util/misc.h"
#include "score/demangle.h"
#include "score/Unit.h"
#include "score_redis/LLRedisClient.h"
#include "score_redis/LLRedisSubscription.h"
#include "score/io/SocketAddr.h"
#include "score_memcached/SyncMCClient.h"
#include "score_memcached/MemcachedConfig.h"

using score::io::SocketAddr;
using namespace score;
using namespace score::redis;
using namespace score::memcached;
using namespace score::func;
using namespace score::async;
using namespace std;


void runRedis() {
  auto ctx = score::util::createShared<EventContext>();
  LOG(INFO) << "made ctx";
  auto client = score::util::createShared<LLRedisClient>(
    ctx.get(), SocketAddr{"127.0.0.1", 6379}
  );
  LOG(INFO) << "made client";

  using conn_result = typename LLRedisClient::connect_result_t;
  using res_t = typename LLRedisClient::response_t;
  client->connect().then([client](conn_result result) {
    LOG(INFO) << "connected.";
    result.throwIfFailed();
    client->set("foo", "bazz", [client](res_t res) {
      LOG(INFO) << "ran set.";
      LOG(INFO) << "set result type: " << res.value().pprint();
      client->get("foo", [client](res_t res2) {
        LOG(INFO) << "get result type: " << res2.value().pprint();
        client->disconnect().then([client](conn_result result) {
          result.throwIfFailed();
        });
      });
    });
  });
  for (;;) {
    ctx->getBase()->runForever();
  }
}

void runMemcached() {
  MemcachedConfig config {{"127.0.0.1", 11211}};
  {
    SyncMCClient client {config};
    client.connect().throwIfFailed();
    client.set("foo", "bar").throwIfFailed();
  }
  {
    SyncMCClient client {config};
    client.connect().throwIfFailed();
    auto result = client.get("foo");
    result.throwIfFailed();
    LOG(INFO) << result.value().value();
  }
}

class SubHandler: public LLRedisSubscription::EventHandler {
 public:
  void onMessage(RedisDynamicResponse&& message) override {
    LOG(INFO) << "onMessage! : " << message.pprint();
  }
  void onStarted() override {
    LOG(INFO) << "started";
  }
  void onStopped() override {
    LOG(INFO) << "stopped";
  }
};

using res_t = typename LLRedisClient::response_t;
using conn_result = typename LLRedisClient::connect_result_t;

void runSubscription() {
  auto ctx = score::util::createShared<EventContext>();
  auto client = score::util::createShared<LLRedisClient>(
    ctx.get(), SocketAddr{"127.0.0.1", 6379}
  );
  using conn_result = typename LLRedisClient::connect_result_t;
  std::shared_ptr<LLRedisSubscription> subPtr {nullptr};
  client->connect().then([client, &subPtr](conn_result result) {
    result.throwIfFailed();
    auto subscription = client->subscribe("some-channel",
      std::make_shared<SubHandler>()
    );
    subscription.throwIfFailed();
    subPtr = subscription.value();
  });
  for (;;) {
    ctx->getBase()->runForever();
  }
}

void logResponse(res_t response) {
  LOG(INFO) << "response: " << response->pprint();
}
void logResponse2(string msg, res_t response) {
  LOG(INFO) << "['" << msg << "'] response: " << response->pprint();
}

// Function<void, res_t> logWith(string msg) {
//   return Function<void, res_t>([msg](res_t response) {
//     logResponse2(msg, response);
//   });
// }
std::function<void (res_t)> logWith(string msg) {
  return std::function<void (res_t)>([msg](res_t response) {
    logResponse2(msg, response);
  });
}

// template<typename TResource>
// std::function<void (res_t)> doWith(TResource resource, Function<void, res_t> callable) {
//   // using arg_t = typename std::remove_reference<func_arg_t>::type;
//   return std::function<void (res_t)>{[resource, callable](res_t arg) {
//     callable(arg);
//   }};
// }

// template<typename TResource>
// Function<void, res_t> doWith(TResource resource, Function<void, res_t> callable) {
//   // using arg_t = typename std::remove_reference<func_arg_t>::type;
//   return Function<void, res_t>{[resource, callable](res_t arg) {
//     callable(arg);
//   }};
// }

template<typename TResource, typename TArg>
Function<void, TArg> doWithImpl(TResource&& resource, Function<void, TArg> &&callable) {
  Function<void, TArg> callMe {std::forward<Function<void, TArg>>(callable)};
  TResource captured {std::forward<TResource>(resource)};
  return Function<void, TArg>{[captured, callMe](TArg arg) {
    callMe(arg);
  }};
}

template<typename TResource, typename TCallable>
auto doWith(TResource&& resource, TCallable&& callable) -> Function<void, typename callable_traits<TCallable>::template nth_arg_type<0>> {
  using arg_t = typename callable_traits<TCallable>::template nth_arg_type<0>;
  Function<void, arg_t> toCall(std::forward<TCallable>(callable));
  return doWithImpl<TResource, arg_t>(
    std::forward<TResource>(resource),
    std::move(toCall)
  );
}

// template<typename TResource, typename TCallable>
// auto doWith(TResource resource, TCallable&& callable)
//   -> Function<void, typename callable_traits<TCallable>::template nth_arg_type<0>::type> {
//   using func_arg_t = typename callable_traits<TCallable>::template nth_arg_type<0>::type;
//   // using arg_t = typename std::remove_reference<func_arg_t>::type;
//   using arg_t = func_arg_t;
//   Function<void, func_arg_t> toCall(std::forward<TCallable>(callable));
//   return Function<void, arg_t>{[resource, toCall](arg_t&& arg) {
//     toCall(std::forward<arg_t>(arg));
//   }};
// }

void runMulti() {
  auto ctx = score::util::createShared<EventContext>();
  auto client = score::util::createShared<LLRedisClient>(
    ctx.get(), SocketAddr{"127.0.0.1", 6379}
  );
  client->connect().then([client](conn_result result) {
    result.throwIfFailed();
    client->get("foo", logWith("GET foo"));
    client->get("bar", doWith(client, logWith("GET bar")));

    client->multi(logWith("MULTI"));
    client->set("x", "x-value", logWith("SET x x-value"));
    client->set("y", "y-value", logWith("SET y y-value"));
    client->get("x", logWith("GET x"));
    client->exec(logWith("EXEC"));

    // client->set("x", "x-value", [client](res_t))
  });
  for (;;) {
    ctx->getBase()->runForever();
  }
}

int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start";
  runMulti();
  LOG(INFO) << "end";
}
