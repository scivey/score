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
  auto client = LLRedisClient::createShared(ctx.get(), "127.0.0.1", 6379);
  client->connect([client](typename LLRedisClient::connect_result_t result) {
    LOG(INFO) << "here.";
    client->set("foo", "bazz", [client](typename LLRedisClient::response_t res) {
      LOG(INFO) << "set result type: " << res.pprint();
      client->get("foo", [](typename LLRedisClient::response_t res2) {
        LOG(INFO) << "get result type: " << res2.pprint();
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
  // runRedis();
  runMemcached();
  LOG(INFO) << "end";
}
