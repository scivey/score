#pragma once

#include <vector>
#include <chrono>
#include <memory>
#include <sys/epoll.h>
#include "score/reactor/EpollFd.h"

namespace score { namespace reactor {

class EpollReactor {
 public:

  using duration_type = std::chrono::milliseconds;
  static const size_t kMaxEvents = 1024;

  class Options {
   protected:
    duration_type waitTimeout_;
   public:
    Options() {
      waitTimeout_ = std::chrono::milliseconds {20};
    }
    void setWaitTimeout(duration_type ms);
    duration_type getWaitTimeout() const;
  };

  class Task {
   protected:
    EpollReactor *reactor_ {nullptr};
    void setReactor(EpollReactor *reactor);
   public:
    friend class EpollReactor;
    EpollReactor* getReactor() const;
    virtual int getFd() = 0;
    virtual void onError() = 0;
    virtual void onReadable() = 0;
    virtual void onWritable() = 0;
    virtual ~Task() = default;
  };

 protected:
  bool running_ {false};
  std::vector<Task*> tasks_;
  epoll_event events_[kMaxEvents];
  EpollFd epollFd_;
  Options options_;
  EpollReactor(EpollFd &&fd, const Options &options);
  static EpollReactor* createPtr(const Options &options);
 public:
  static EpollReactor create(const Options &options = Options());
  static std::unique_ptr<EpollReactor> createUnique(const Options &options = Options());
  void addTask(Task *task);
  int runOnce();
  void loopForever();
  void runForDuration(duration_type minDuration);
  void stop();
  bool isRunning() const;
};




}} // score::reactor