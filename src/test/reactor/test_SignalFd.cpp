#include <gtest/gtest.h>
#include <atomic>
#include <memory>
#include <sys/timerfd.h>

#include "aliens/reactor/ReactorThread.h"
#include "aliens/posix/FileDescriptor.h"
#include "aliens/reactor/SignalFd.h"
#include "aliens/locks/Synchronized.h"
#include "aliens/exceptions/macros.h"
#include "aliens/mem/util.h"

using namespace aliens::mem;
using namespace aliens::async;
using namespace aliens::reactor;
using namespace aliens::locks;
using namespace aliens::posix;
using namespace std;


namespace {

class Handler: public SignalFd::EventHandler {
 protected:
  Synchronized<std::vector<uint32_t>> received_;
 public:
  void onSignal(uint32_t sig) {
    received_.getHandle()->push_back(sig);
  }
  std::vector<uint32_t> copySignals() {
    std::vector<uint32_t> output;
    auto handle = received_.getHandle();
    for (auto sig: *handle) {
      output.push_back(sig);
    }
    return output;
  }
};

}

namespace {
void joinAtomic(std::atomic<bool> &done) {
  while (!done.load()) {
    ;
  }
}
}

TEST(TestSignalFd, TestSanity) {
  auto reactorThread = ReactorThread::createShared();
  reactorThread->start();
  std::shared_ptr<Handler> handler(new Handler);
  auto sigFd = SignalFd::Factory::createShared(handler.get());
  std::atomic<bool> added {false};
  reactorThread->addTask(sigFd->getEpollTask(), [&added](){
    added.store(true);
  });
  joinAtomic(added);
  std::atomic<bool> ended {false};
  reactorThread->runInEventThread([&ended, reactorThread](){
    reactorThread->stop([reactorThread, &ended](const ErrBack::except_option &err) {
      EXPECT_FALSE(err.hasValue());
      ended.store(true);
    });
  });
  joinAtomic(ended);
  reactorThread->join();
}