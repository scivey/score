#include "aliens/reactor/ReactorThread.h"

namespace aliens { namespace reactor {

ReactorThread::ReactorThread(){}

std::shared_ptr<ReactorThread> ReactorThread::createShared() {
  std::shared_ptr<ReactorThread> instance{new ReactorThread};
  instance->reactor_ = EpollReactor::createUnique();
  return instance;
}

EpollReactor* ReactorThread::getReactor() {
  return reactor_.get();
}

bool ReactorThread::isRunning() const {
  return running_.load();
}

void ReactorThread::start() {
  CHECK(!isRunning());
  bool expected = false;
  bool desired = true;
  if (running_.compare_exchange_strong(expected, desired)) {
    auto self = shared_from_this();
    thread_.reset(new std::thread([this, self]() {
      while (isRunning()) {
        reactor_->runForDuration(std::chrono::milliseconds(50));
        auto queueHandle = toRun_.getHandle();
        while (!queueHandle->empty()) {
          async::VoidCallback func = std::move(queueHandle->front());
          queueHandle->pop();
          func();
        }
      }
      if (onStopped_.hasValue()) {
        VLOG(50) << "calling onStopped";
        onStopped_.value()();
      } else {
        VLOG(50) << "not calling onStopped.";
      }
      finished_.store(true);
    }));
  } else {
    throw exceptions::BaseError("already started!");
  }
}

void ReactorThread::stop(async::ErrBack &&cb) {
  if (!isRunning()) {
    cb(exceptions::BaseError("not running!"));
    return;
  }
  onStopped_.assign(std::move(cb));
  bool expected = true;
  bool desired = false;
  if (running_.compare_exchange_strong(expected, desired)) {
    ; // do nothing; cb will be called by the loop in `start()`
  } else {
    onStopped_.value()(exceptions::BaseError("someone else stopped before we could. race condition?"));
  }
}

void ReactorThread::addTaskImpl(EpollReactor::Task *task) {
  reactor_->addTask(task);
}

void ReactorThread::addTask(EpollReactor::Task *task) {
  auto self = shared_from_this();
  runInEventThread([self, this, task]() {
    addTaskImpl(task);
  });
}

void ReactorThread::addTask(EpollReactor::Task *task, async::VoidCallback &&cb) {
  auto self = shared_from_this();
  auto cbWrapper = aliens::makeMoveWrapper(std::forward<async::VoidCallback>(cb));
  runInEventThread([self, this, task, cbWrapper]() {
    addTaskImpl(task);
    aliens::MoveWrapper<async::VoidCallback> unwrapped = cbWrapper;
    async::VoidCallback movedCb = unwrapped.move();
    movedCb();
  });
}

void ReactorThread::runInEventThread(async::VoidCallback &&cb) {
  auto handle = toRun_.getHandle();
  handle->push(std::move(cb));
}

void ReactorThread::runInEventThread(async::VoidCallback &&cb, async::ErrBack &&onFinish) {
  auto cbWrapper = aliens::makeMoveWrapper(
    std::forward<async::VoidCallback>(cb)
  );
  auto doneWrapper = aliens::makeMoveWrapper(
    std::forward<async::ErrBack>(onFinish)
  );
  auto self = shared_from_this();
  runInEventThread([this, self, cbWrapper, doneWrapper]() {
    MoveWrapper<async::VoidCallback> movedCb = cbWrapper;
    async::VoidCallback unwrappedCb = movedCb.move();
    MoveWrapper<async::ErrBack> movedDoneCb = doneWrapper;
    async::ErrBack unwrappedDoneCb = movedDoneCb.move();
    bool raised = false;
    try {
      unwrappedCb();
    } catch (const std::exception &ex) {
      raised = true;
      unwrappedDoneCb(ex);
    }
    if (!raised) {
      unwrappedDoneCb();
    }
  });
}

void ReactorThread::join() {
  if (!isRunning() && joined_.load()) {
    return;
  }
  while (running_.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  while (!finished_.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  {
    bool expected = false;
    bool desired = true;
    if (joining_.compare_exchange_strong(expected, desired)) {
      thread_->join();
      joined_.store(true);
    } else {
      while (!joined_.load()) {
        ;
      }
    }
  }
}

ReactorThread::~ReactorThread() {
  join();
}

}} // aliens::reactor
