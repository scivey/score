#include <glog/logging.h>
// #include <spdlog/spdlog.h>
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/ReactorThread.h"
#include "aliens/reactor/SignalFd.h"
#include "aliens/reactor/TimerFd.h"
#include "aliens/reactor/TimerSettings.h"
#include "aliens/async/ErrBack.h"
#include "aliens/async/VoidCallback.h"

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

int main() {
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
  this_thread::sleep_for(chrono::milliseconds(4000));
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
