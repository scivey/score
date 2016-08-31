#include <gtest/gtest.h>
#include <atomic>
#include <vector>
#include <glog/logging.h>
#include "score/reactor/EventFd.h"
#include "score/reactor/ReactorThread.h"
#include "score/async/ErrBack.h"
#include "score/PointerFactory.h"

#include "score/mem/util.h"
#include "score/locks/Synchronized.h"

using namespace score::reactor;
using namespace score::async;
using namespace score::mem;
using namespace score::locks;
using namespace std;
namespace {

class EventHandler : public EventFd::EventHandler {
 protected:
  // already protected by lock in `Synchronized`.
  mutable Synchronized<std::vector<uint64_t>> events_;

 public:
  size_t numReceived() const {
    return events_.getHandle()->size();
  }
  void onEvent(uint64_t evt) override {
    events_.getHandle()->push_back(evt);
  }
  std::vector<uint64_t> copyEvents() const {
    auto handle = events_.getHandle();
    std::vector<uint64_t> result;
    for (auto item: *handle) {
      result.push_back(item);
    }
    return result;
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
  std::shared_ptr<EventHandler> handler(new EventHandler);
  auto evFd = EventFd::Factory::createShared(handler.get());
  std::atomic<bool> added {false};
  reactorThread->addTask(evFd->getEpollTask(), [&added, evFd, reactorThread](){
    added.store(true);
  });
  joinAtomic(added);
  std::atomic<bool> sent {false};
  reactorThread->runInEventThread([reactorThread, evFd, &sent]() {
    evFd->write(957);
    sent.store(true);
  });
  joinAtomic(sent);
  while (handler->numReceived() == 0) {
    ;
  }
  auto received = handler->copyEvents();
  EXPECT_EQ(957, received.at(0));
  EXPECT_EQ(1, received.size());
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