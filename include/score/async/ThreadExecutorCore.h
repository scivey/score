#pragma once
#include <atomic>
#include <thread>
#include <queue>
#include "score/async/ExecutorBase.h"
#include "score/NullablePointer.h"
#include "score/Maybe.h"
#include "score/macros.h"
#include "score/locks/Synchronized.h"


namespace score { namespace async {

class ThreadExecutorCore :
          public Executor,
          public std::enable_shared_from_this<ThreadExecutorCore> {
 protected:
  struct Task {
    VoidCallback work;
    ErrBack resultCb;
  };
  Maybe<std::thread> thread_;
  locks::Synchronized<std::queue<Task>> queue_;
  std::atomic<bool> running_ {false};
  Maybe<VoidCallback> onClosed_;
  Maybe<VoidCallback> onStarted_;
  void runTask(Task &task) {
    bool failed = false;
    try {
      task.work();
    } catch (const std::exception &ex) {
      task.resultCb(ex);
      failed = true;
    }
    if (!failed) {
      task.resultCb();
    }
  }
  void runLoop() {
    SDCHECK(running_.load());
    if (onStarted_.hasValue()) {
      onStarted_.value()();
    }
    for (;;) {
      if (!running_.load()) {
        break;
      }
      auto handle = queue_.getHandle();
      if (handle->empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
      } else {
        Task item = std::move(handle->front());
        runTask(item);
      }
    }
    if (onClosed_.hasValue()) {
      onClosed_.value()();
    }
  }
 public:
  bool isRunning() const {
    return running_.load();
  }
  virtual void stop(ErrBack &&errBack) override {
    if (!running_.load()) {
      errBack(std::runtime_error {"stop() called on non-running instance."});
    } else {
      SDCHECK(!onClosed_.hasValue());
      auto wrapper = makeMoveWrapper(errBack);
      auto self = shared_from_this();
      onClosed_.assign([this, wrapper, self]() {
        MoveWrapper<ErrBack> wrapped = wrapper;
        ErrBack unwrapped = wrapped.move();
        unwrapped();
      });
      running_.store(false);
    }
  }
  void join(ErrBack &&errBack) {
    auto self = shared_from_this();
    auto cbWrapper = makeMoveWrapper(std::move(errBack));
    stop([self, this, cbWrapper](const ErrBack::except_option &err) {
      MoveWrapper<ErrBack> unwrapped = cbWrapper;
      ErrBack cb = unwrapped.move();
      if (err) {
        cb(err);
        return;
      }
      SDCHECK(!err.hasValue());
      if (thread_.hasValue()) {
        thread_.value().join();
      }
      cb(err);
    });
  }
  void maybeJoin() {
    if (thread_.hasValue()) {
      thread_.value().join();
      thread_.reset();
    }
  }
  virtual void start(ErrBack &&errBack) override {
    LOG(INFO) << "here..";
    if (running_.load()) {
      errBack(std::runtime_error {"start() called on already-running instance."});
    } else {
      SDCHECK(!onStarted_.hasValue());
      auto wrapper = makeMoveWrapper(std::move(errBack));
      onStarted_.assign([this, wrapper]() {
        MoveWrapper<ErrBack> wrapped = wrapper;
        ErrBack unwrapped = wrapped.move();
        unwrapped();
      });
      running_.store(true);
      thread_.assign(std::thread([this]() {
        runLoop();
      }));
    }
  }
  virtual void submit(VoidCallback &&work, ErrBack &&errBack) override {
    Task task;
    task.work = std::move(work);
    task.resultCb = std::move(errBack);
    queue_.getHandle()->push(std::move(task));
  }
  ~ThreadExecutorCore() {
    LOG(INFO) << "~ThreadExecutorCore 1";
    maybeJoin();
    LOG(INFO) << "~ThreadExecutorCore 2";

  }
};


}} // score::async