#include <gtest/gtest.h>
#include <atomic>
#include <memory>
#include <chrono>
#include "aliens/posix/FileDescriptor.h"
#include "aliens/reactor/ReactorThread.h"
#include "aliens/reactor/TimerFd.h"
#include "aliens/exceptions/macros.h"
#include "aliens/FixedBuffer.h"
#include "aliens/mem/util.h"


using namespace aliens::mem;
using namespace aliens::async;
using namespace aliens::posix;
using namespace aliens::reactor;
using namespace std;
using aliens::Buffer;



TEST(TestReactorExperimental, TestSomething) {
  // auto reactor = ReactorThread::createShared();
  // TimerSettings settings {
  //   chrono::milliseconds {1000},
  //   chrono::milliseconds {1000}
  // };
  // auto timerFd = TimerFd::createShared()
  EXPECT_TRUE(true);
}