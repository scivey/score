#include <gtest/gtest.h>
#include <atomic>
#include <memory>
#include <chrono>
#include "score/posix/FileDescriptor.h"
#include "score/reactor/ReactorThread.h"
#include "score/reactor/TimerFd.h"
#include "score/exceptions/macros.h"
#include "score/FixedBuffer.h"
#include "score/mem/util.h"


using namespace score::mem;
using namespace score::async;
using namespace score::posix;
using namespace score::reactor;
using namespace std;
using score::Buffer;



TEST(TestReactorExperimental, TestSomething) {
  // auto reactor = ReactorThread::createShared();
  // TimerSettings settings {
  //   chrono::milliseconds {1000},
  //   chrono::milliseconds {1000}
  // };
  // auto timerFd = TimerFd::createShared()
  EXPECT_TRUE(true);
}