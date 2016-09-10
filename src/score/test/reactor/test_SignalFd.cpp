#include <gtest/gtest.h>
#include <atomic>
#include <memory>
#include <sys/timerfd.h>

#include "score/reactor/ReactorThread.h"
#include "score/posix/FileDescriptor.h"
#include "score/reactor/SignalFd.h"
#include "score/locks/Synchronized.h"
#include "score/exceptions/macros.h"
#include "score/mem/util.h"

using namespace score::mem;
using namespace score::async;
using namespace score::reactor;
using namespace score::locks;
using namespace score::posix;
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