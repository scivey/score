#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <glog/logging.h>
#include "aliens/atomic/AtomicUint128.h"
using namespace std;
using namespace aliens::atomic;


TEST(TestIntrinsicAtomic, TestAtomicUint128) {
  AtomicUint128 anAtom;
  std::vector<std::shared_ptr<thread>> threads;
  const size_t kNThreads = 8;
  const size_t kIncrementsPerThread = 100;
  for (size_t i = 0; i < kNThreads; i++) {
    threads.push_back(std::make_shared<std::thread>(
      [&anAtom, kIncrementsPerThread]() {
        for (size_t j = 0; j < kIncrementsPerThread; j++) {
          for (;;) {
            auto expected = anAtom.load();
            auto desired = expected + 1;
            if (anAtom.compare_exchange_strong(&expected, desired)) {
              break;
            }
          }
        }
      }
    ));
  }
  for (auto &t: threads) {
    t->join();
  }
  auto lastVal = anAtom.load();
  auto lowBytes = (uint64_t) lastVal;
  EXPECT_EQ(kNThreads * kIncrementsPerThread, lowBytes);
  auto highBytes = lastVal >> 64;
  EXPECT_EQ(0, highBytes);
}
