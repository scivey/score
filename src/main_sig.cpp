#include <glog/logging.h>
// #include <spdlog/spdlog.h>
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/ReactorThread.h"
#include "aliens/reactor/SignalFd.h"
#include "aliens/reactor/TimerFd.h"
#include "aliens/reactor/EventFd.h"

#include "aliens/reactor/TimerSettings.h"
#include "aliens/async/ErrBack.h"
#include "aliens/async/VoidCallback.h"
#include "aliens/async/Callback.h"
#include "aliens/exceptions/macros.h"
#include <signal.h>

using namespace std;
using namespace aliens::async;
using namespace aliens::reactor;

class CallbackTimerHandler : public TimerFd::EventHandler {
 protected:
  VoidCallback callback_;
 public:
  CallbackTimerHandler(VoidCallback &&cb)
    : callback_(std::forward<VoidCallback>(cb)) {}
  void onTick() override {
    callback_();
  }
};


class CallbackEventFdHandler : public EventFd::EventHandler {
 public:
  using callback_type = Callback<uint64_t>;
 protected:
  callback_type callback_;
 public:
  CallbackEventFdHandler(callback_type &&cb)
    : callback_(std::forward<callback_type>(cb)) {}
  void onEvent(uint64_t evt) override {
    callback_(evt);
  }
};

class CallbackSignalHandler : public SignalFd::EventHandler {
 public:
  using callback_type = Callback<uint32_t>;
 protected:
  callback_type callback_;
 public:
  CallbackSignalHandler(callback_type &&cb)
    : callback_(std::forward<callback_type>(cb)) {}
  void onSignal(uint32_t signo) override {
    callback_(signo);
  }
};

int main() {
  sigset_t mask;
  int sfd;
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  sigaddset(&mask, SIGUSR1);
  ALIENS_CHECK_SYSCALL2(
    sigprocmask(SIG_BLOCK, &mask, nullptr),
    "sigprocmask()"
  );
  google::InstallFailureSignalHandler();
  auto react = ReactorThread::createShared();
  react->start();
  auto handler1 = std::make_shared<CallbackTimerHandler>(VoidCallback([]() {
    LOG(INFO) << "TICK1!";
  }));
  auto handler2 = std::make_shared<CallbackTimerHandler>(VoidCallback([]() {
    LOG(INFO) << "TICK2!";
  }));
  auto timer1 = TimerFd::createShared(
    TimerSettings(
      std::chrono::milliseconds(500),
      std::chrono::milliseconds(1000)
    ),
    handler1.get()
  );
  auto timer2 = TimerFd::createShared(
    TimerSettings(
      std::chrono::milliseconds(1000),
      std::chrono::milliseconds(500)
    ),
    handler2.get()
  );

  react->addTask(timer1->getEpollTask(), [](){
    LOG(INFO) << "added task1...";
  });
  react->addTask(timer2->getEpollTask(), [](){
    LOG(INFO) << "added task2...";
  });

  std::atomic<size_t> sigIntCount {0};
  auto sigHandler = std::make_shared<CallbackSignalHandler>([&sigIntCount](uint32_t sig) {
    LOG(INFO) << "SIGNAL : " << sig;
    if (sig == SIGINT) {
      auto prev = sigIntCount.fetch_add(1);
      if (prev >= 2) {
        LOG(INFO) << "got 3 SIGINTs; exiting.";
        exit(1);
      }
    }
  });
  auto sigFd = SignalFd::createShared(sigHandler.get());
  react->addTask(sigFd->getEpollTask(), []() {
    LOG(INFO) << "added sigHandler.";
  });

  auto evHandler = std::make_shared<CallbackEventFdHandler>([](uint64_t evt) {
    LOG(INFO) << "event : " << evt;
  });
  auto evFd = EventFd::createShared(evHandler.get());
  std::atomic<bool> addedEv {false};
  react->addTask(evFd->getEpollTask(), [&addedEv]() {
    LOG(INFO) << "added evHandler.";
    addedEv.store(true);
  });
  while (!addedEv.load()) {
    this_thread::sleep_for(chrono::milliseconds(10));
  }
  react->runInEventThread([evFd, react]() {
    evFd->write(817);
  });

  this_thread::sleep_for(chrono::milliseconds(30000));
  react->stop([react](const ErrBack::except_option &err) {
    if (err.hasValue()) {
      LOG(INFO) << "ERR : " << err.value().what();
    } else {
      LOG(INFO) << "stopped with no errors..";
    }
  });
  react->join();
  LOG(INFO) << "here...";
}
