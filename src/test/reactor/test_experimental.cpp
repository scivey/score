#include <gtest/gtest.h>
#include <atomic>
#include <memory>
#include <sys/timerfd.h>

#include "aliens/reactor/ReactorThread.h"
#include "aliens/reactor/FileDescriptor.h"
#include "aliens/reactor/ClientSocketTask.h"
#include "aliens/reactor/AcceptSocketTask.h"
#include "aliens/exceptions/macros.h"
#include "aliens/FixedBuffer.h"
#include "aliens/mem/util.h"

using namespace aliens::mem;
using namespace aliens::async;

using namespace std;
using aliens::Buffer;


namespace aliens { namespace reactor {


void convertTime(timespec *target, std::chrono::milliseconds msec) {
  size_t sec = msec.count() / 1000;
  target->tv_sec = sec;
  size_t remainingMsec = msec.count() - (sec * 1000);
  if (remainingMsec) {
    target->tv_nsec = remainingMsec * 1000000;
  } else {
    target->tv_nsec = 0;
  }
}

class TimerSettings {
 public:
  using duration_type = std::chrono::milliseconds;
 protected:
  duration_type initialDelay_ {0};
  duration_type intervalDuration_ {0};
  itimerspec timerSpec_;
  void updateTimerSpec();
public:
  TimerSettings();
  TimerSettings(duration_type initial, duration_type interval);
  itimerspec* getTimerSpec();
};

void TimerSettings::updateTimerSpec() {
  convertTime(&timerSpec_.it_value, initialDelay_);
  convertTime(&timerSpec_.it_interval, intervalDuration_);
}
TimerSettings::TimerSettings() {
  memset(&timerSpec_, '\0', sizeof(timerSpec_));
}

TimerSettings::TimerSettings(
  typename TimerSettings::duration_type init,
  typename TimerSettings::duration_type interval)
  : initialDelay_(init), intervalDuration_(interval) {
  updateTimerSpec();
}
itimerspec* TimerSettings::getTimerSpec() {
  return &timerSpec_;
}

class TimerFd {
 public:
  class EpollTask : public EpollReactor::Task {
   protected:
    TimerFd* parent_ {nullptr};
    EpollTask(){}
    void setParent(TimerFd *parent) {
      parent_ = parent;
    }
    friend class TimerFd;
    TimerFd* getParent() const {
      return parent_;
    }
   public:
    void onReadable() override;
    void onWritable() override;
    void onError() override;
    int getFd() override;
  };

  friend class EpollTask;

  class EventHandler {
   public:
    virtual void onTick() = 0;
    virtual ~EventHandler() = default;
  };
 protected:
  FileDescriptor fd_;
  TimerSettings settings_;
  EpollTask epollTask_;
  EventHandler *handler_ {nullptr};

  TimerFd(FileDescriptor &&desc, EventHandler *handler);
  void triggerRead();
 public:
  EpollTask *getEpollTask() {
    return &epollTask_;
  }
  int getFdNo() const {
    return fd_.getFdNo();
  }
  static TimerFd create(const TimerSettings&, EventHandler *handler);
  static std::shared_ptr<TimerFd> createShared(const TimerSettings&, EventHandler *handler);
  void stop();
};


TimerFd::TimerFd(FileDescriptor &&desc, TimerFd::EventHandler *handler)
  : fd_(std::forward<FileDescriptor>(desc)),
    handler_(handler) {
  epollTask_.setParent(this);
}

void TimerFd::stop() {
  fd_.close();
}

void TimerFd::triggerRead() {
  uint64_t nTimeouts {0};
  CHECK(8 == read(getFdNo(), &nTimeouts, sizeof(uint64_t)));
  CHECK(!!handler_);

  // nTimeouts should almost always be 1.
  // are there edge cases?
  for (size_t i = 0; i < nTimeouts; i++) {
    handler_->onTick();
  }
}

TimerFd TimerFd::create(const TimerSettings& settings, TimerFd::EventHandler *handler) {
  int fd = timerfd_create(
    CLOCK_MONOTONIC,
    TFD_NONBLOCK | TFD_CLOEXEC
  );
  ALIENS_CHECK_SYSCALL2(fd, "TimerFd::create()");
  TimerFd instance(FileDescriptor::fromIntExcept(fd), handler);
  instance.settings_ = settings;
  itimerspec previousSettings;
  itimerspec *desiredSettings = instance.settings_.getTimerSpec();

  const int kRelativeTimerFlag = 0;
  ALIENS_CHECK_SYSCALL2(
    timerfd_settime(
      instance.getFdNo(),
      kRelativeTimerFlag,
      desiredSettings,
      &previousSettings
    ),
    "timerfd_settime()"
  );
  return instance;
}

std::shared_ptr<TimerFd> TimerFd::createShared(
    const TimerSettings& settings, TimerFd::EventHandler *handler) {
  return std::shared_ptr<TimerFd>(new TimerFd(create(
    settings, handler
  )));
}

void TimerFd::EpollTask::onReadable() {
  getParent()->triggerRead();
}

void TimerFd::EpollTask::onWritable() {
  LOG(INFO) << "TimerFd::EpollTask::onWritable()";
}

void TimerFd::EpollTask::onError() {
  LOG(INFO) << "TimerFd::EpollTask::onError()";
}

int TimerFd::EpollTask::getFd() {
  return parent_->getFdNo();
}


}} // aliens::reactor


using namespace aliens::reactor;

class Handler : public TimerFd::EventHandler {
 protected:
  std::atomic<size_t> &target_;
 public:
  Handler(std::atomic<size_t> &target): target_(target){}
  void onTick() override {
    target_.fetch_add(1);
  }
};



namespace {
void joinAtomic(std::atomic<bool> &done) {
  while (!done.load()) {
    ;
  }
}
}

TEST(TestReactorExperimental, TestTimerFd) {
  auto reactorThread = ReactorThread::createShared();
  reactorThread->start();
  std::atomic<size_t> target {0};
  auto handler = makeUnique<Handler>(target);
  auto timer = TimerFd::createShared(
    TimerSettings(chrono::milliseconds(5), chrono::milliseconds(5)),
    handler.get()
  );
  reactorThread->addTask(timer->getEpollTask(), [](){
    LOG(INFO) << "added task!";
  });

  size_t current = target.load();
  while (current < 5) {
    ;
    current = target.load();
  }
  std::atomic<bool> finished {false};
  reactorThread->runInEventThread([timer, reactorThread, &finished]() {
    timer->stop();
    reactorThread->stop([timer, reactorThread, &finished](const ErrBack::except_option &err) {
      EXPECT_FALSE(err.hasValue());
      finished.store(true);
    });
  });
  joinAtomic(finished);
  LOG(INFO) << "waiting for stop..";
  reactorThread->join();
  EXPECT_GT(target.load(), 4);
  LOG(INFO) << "end.";
}