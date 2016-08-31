#pragma once
#include <memory>
#include <atomic>
#include <chrono>
#include <queue>
#include <thread>
#include "score/locks/Synchronized.h"
#include "score/reactor/EpollReactor.h"
#include "score/Maybe.h"
#include "score/MoveWrapper.h"
#include "score/async/ErrBack.h"
#include "score/async/VoidCallback.h"
#include "score/exceptions/exceptions.h"

namespace score { namespace reactor {

class ReactorThread : public std::enable_shared_from_this<ReactorThread> {
 protected:
  std::unique_ptr<EpollReactor> reactor_ {nullptr};
  std::unique_ptr<std::thread> thread_ {nullptr};
  ReactorThread();
  std::atomic<bool> running_ {false};
  std::atomic<bool> finished_ {false};
  std::atomic<bool> joining_ {false};
  std::atomic<bool> joined_ {false};
  score::Maybe<async::ErrBack> onStopped_;
  locks::Synchronized<std::queue<async::VoidCallback>> toRun_;
 public:
  static std::shared_ptr<ReactorThread> createShared();
  EpollReactor* getReactor();
  bool isRunning() const;
  void start(const EpollReactor::Options& = EpollReactor::Options());
  void stop(async::ErrBack &&cb);

 protected:
  // NB must be called from event loop thread.
  void addTaskImpl(EpollReactor::Task *task);

 public:
  void addTask(EpollReactor::Task *task);
  void addTask(EpollReactor::Task *task, async::VoidCallback &&cb);
  void runInEventThread(async::VoidCallback &&cb);
  void runInEventThread(async::VoidCallback &&cb, async::ErrBack &&onFinish);
  void join();
  ~ReactorThread();
};

}} // score::reactor
