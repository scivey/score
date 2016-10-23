#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include <chrono>
#include "score_async/EventContext.h"

using namespace score;
using namespace score::async;
namespace util = score::util;
using namespace std;
using ControlMessage = EventContext::ControlMessage;

struct TestMessage {
  thread::id sentFrom {0};
  thread::id ranIn {0};
  TestMessage(){}
  TestMessage(thread::id sender, thread::id runner)
    : sentFrom(sender), ranIn(runner) {}
};

TEST(TestEventContext, TestControlMessages) {
  auto evCtx = util::createShared<EventContext>();
  std::atomic<bool> done {false};
  std::vector<TestMessage> results;
  std::thread::id baseId = this_thread::get_id();

  const size_t kNumThreads = 8;
  std::vector<std::shared_ptr<thread>> threads;
  for (size_t i = 0; i < kNumThreads; i++) {
    threads.push_back(std::make_shared<std::thread>(
      [evCtx, &results, kNumThreads, baseId, &done]() {
        auto senderId = this_thread::get_id();
        evCtx->threadsafeTrySendControlMessage(ControlMessage{
          [senderId, &results, baseId, kNumThreads, &done]() {
            auto runnerId = this_thread::get_id();
            EXPECT_EQ(baseId, runnerId);
            results.push_back(TestMessage{senderId, runnerId});
            if (results.size() >= kNumThreads) {
              done.store(true);
            }
          }
        }).throwIfFailed();
      }
    ));
  }
  while (!done.load()) {
    evCtx->getBase()->runOnce();
  }
  for (auto& t: threads) {
    t->join();
  }
  EXPECT_EQ(kNumThreads, results.size());
  std::set<thread::id> seen;
  for (auto& msg: results) {
    EXPECT_EQ(baseId, msg.ranIn);
    seen.insert(msg.sentFrom);
  }
  EXPECT_EQ(kNumThreads, seen.size());
  EXPECT_EQ(0, seen.count(baseId));
}
