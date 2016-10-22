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

using namespace score;
using namespace score::redis;
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

int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start";
  runRedis();
  LOG(INFO) << "end";
}
