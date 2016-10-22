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

#include "score_memcached/SyncMCClient.h"
#include "score_memcached/MemcachedConfig.h"

using namespace score;
using namespace score::redis;
using namespace score::memcached;

using namespace score::async;
using namespace std;


void runRedis() {
  auto ctx = score::util::createShared<EventContext>();
  LOG(INFO) << "made ctx";
  auto client = score::util::createShared<LLRedisClient>(ctx.get(), "127.0.0.1", 6379);
  LOG(INFO) << "made client";

  using conn_result = typename LLRedisClient::connect_result_t;
  using res_t = typename LLRedisClient::response_t;
  client->connect().then([client](conn_result result) {
    LOG(INFO) << "connected.";
    result.throwIfFailed();
    client->set("foo", "bazz", [client](res_t res) {
      LOG(INFO) << "ran set.";
      LOG(INFO) << "set result type: " << res.pprint();
      client->get("foo", [client](res_t res2) {
        LOG(INFO) << "get result type: " << res2.pprint();
        client->disconnect().then([client](conn_result result) {
          LOG(INFO) << "disconnected! 1";
          result.throwIfFailed();
          LOG(INFO) << "disconnected! 2";
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

int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start";
  runRedis();
  // runMemcached();
  LOG(INFO) << "end";
}
