#include <gtest/gtest.h>
#include <atomic>
#include <glog/logging.h>
#include "aliens/reactor/EventFd.h"
#include "aliens/reactor/ReactorThread.h"
#include "aliens/async/ErrBack.h"
#include "aliens/mem/util.h"

using namespace aliens::reactor;
using namespace aliens::async;
using namespace aliens::mem;

namespace {

class EventHandler : public EventFd::EventHandler {
 public:
  void onTick() override {
    LOG(INFO) << "onTick";
  }
};

void joinAtomic(std::atomic<bool> &done) {
  while (!done.load()) {
    ;
  }
}
}

TEST(EventFd, TestSanity) {
  auto reactorThread = ReactorThread::createShared();
  reactorThread->start();
  auto handler = makeUnique<EventHandler>();
  auto evFd = EventFd::createShared(handler.get());
  std::atomic<bool> added {false};
  reactorThread->addTask(evFd->getEpollTask(), [&added, evFd, reactorThread](){
    added.store(true);
  });
  joinAtomic(added);
  std::atomic<bool> stopped {false};
  reactorThread->runInEventThread([reactorThread, evFd, &stopped]() {
    reactorThread->stop([reactorThread, evFd, &stopped](const ErrBack::except_option& err) {
      EXPECT_FALSE(err.hasValue());
      stopped.store(true);
    });
  });
  joinAtomic(stopped);
  reactorThread->join();
}