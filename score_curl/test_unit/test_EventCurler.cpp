#include <gtest/gtest.h>
#include <functional>
#include <atomic>
#include <memory>
#include <string>
#include "score/util/misc.h"
#include "score/curl/EventCurler.h"
#include "score/async/EventContext.h"

using namespace std;
using score::async::EventContext;
using score::curl::EventCurler;
using score::TimerSettings;


namespace util = score::util;

TEST(TestEventCurler, TestSanity) {
  auto ctx = util::createShared<EventContext>();
  auto curler = util::createShared<EventCurler>(ctx.get());
  bool done {false};
  curler->getURL("http://localhost:80", [&done](string result) {
    EXPECT_TRUE(result.size() > 0);
    done = true;
  });
  TimerSettings timeSettings {std::chrono::milliseconds {10}};
  auto timeVal = timeSettings.toTimeVal();
  while (!done) {
    ctx->getBase()->runFor(&timeVal);
  }
  EXPECT_TRUE(done);
}
