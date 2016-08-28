#include <gtest/gtest.h>
#include <atomic>
#include <vector>
#include <glog/logging.h>
#include "aliens/reactor/EventFd.h"
#include "aliens/reactor/ReactorThread.h"
#include "aliens/async/ErrBack.h"
#include "aliens/mem/util.h"
#include "aliens/locks/Synchronized.h"

using namespace aliens::reactor;
using namespace aliens::async;
using namespace aliens::mem;
using namespace aliens::locks;
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
    LOG(INFO) << "onEvent : " << evt;
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

class Barrier {
 protected:
  std::atomic<size_t> counter_ {0};
  size_t expected_ {0};
  std::atomic<bool> done_ {false};
 public:
  Barrier(size_t expected): expected_(expected){}
  void post() {
    size_t prev = counter_.fetch_add(1);
    LOG(INFO) << "post : " << prev;
    if (prev == expected_ - 1) {
      done_.store(true);
    }
  }
  void wait() {
    while(!done_.load()) {
      ;
    }
  }
};

TEST(EventFd, TestSanity) {
  auto reactorThread = ReactorThread::createShared();
  reactorThread->start();
  std::shared_ptr<EventHandler> handler(new EventHandler);
  auto evFd = EventFd::createShared(handler.get());
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
  LOG(INFO) << "numReceived adequate.";
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