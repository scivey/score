#include <gtest/gtest.h>
#include <atomic>
#include <memory>
#include <sys/timerfd.h>

#include "aliens/reactor/ReactorThread.h"
#include "aliens/reactor/FileDescriptor.h"
#include "aliens/reactor/ClientSocketTask.h"
#include "aliens/reactor/AcceptSocketTask.h"
#include "aliens/reactor/TimerFd.h"
#include "aliens/exceptions/macros.h"
#include "aliens/FixedBuffer.h"
#include "aliens/mem/util.h"

using namespace aliens::mem;
using namespace aliens::async;

using namespace std;
using aliens::Buffer;


using namespace aliens::reactor;


namespace {
void joinAtomic(std::atomic<bool> &done) {
  while (!done.load()) {
    ;
  }
}
}

TEST(TestReactorExperimental, TestSomething) {
  EXPECT_TRUE(true);
}