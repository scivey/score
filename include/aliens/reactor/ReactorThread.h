#pragma once
#include <memory>
#include <atomic>
#include <chrono>
#include <queue>
#include <thread>
#include "aliens/locks/Synchronized.h"
#include "aliens/reactor/EpollReactor.h"
#include "aliens/Maybe.h"
#include "aliens/MoveWrapper.h"
#include "aliens/async/ErrBack.h"
#include "aliens/async/VoidCallback.h"
#include "aliens/exceptions/exceptions.h"

namespace aliens { namespace reactor {

class ReactorThread : public std::enable_shared_from_this<ReactorThread> {
 protected:
  std::unique_ptr<EpollReactor> reactor_ {nullptr};
  std::unique_ptr<std::thread> thread_ {nullptr};
  ReactorThread();
  std::atomic<bool> running_ {false};
  aliens::Maybe<async::ErrBack> onStopped_;
  locks::Synchronized<std::queue<async::VoidCallback>> toRun_;
 public:
  static std::shared_ptr<ReactorThread> createShared();
  EpollReactor* getReactor();
  bool isRunning() const;
  void start();
  void stop(async::ErrBack &&cb);

  // NB must be called from event loop thread.
  void addTaskImpl(EpollReactor::Task *task);
  void addTask(EpollReactor::Task *task);
  void addTask(EpollReactor::Task *task, async::VoidCallback &&cb);
  void runInEventThread(async::VoidCallback &&cb);
  void runInEventThread(async::VoidCallback &&cb, async::ErrBack &&onFinish);
  void join();
  ~ReactorThread();
};

}} // aliens::reactor
