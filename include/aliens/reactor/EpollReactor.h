#pragma once

#include <vector>
#include <chrono>
#include <memory>
#include <sys/epoll.h>
#include "aliens/reactor/EpollFd.h"

namespace aliens { namespace reactor {

class EpollReactor {
 public:
  static const size_t kMaxEvents = 1024;
  class Task {
   protected:
    EpollReactor *reactor_ {nullptr};
    void setReactor(EpollReactor *reactor);
   public:
    friend class EpollReactor;
    EpollReactor* getReactor() const;
    virtual int getFd() = 0;
    virtual void onError() = 0;
    virtual void onEvent() = 0;
  };
 protected:
  bool running_ {false};
  std::vector<Task*> tasks_;
  epoll_event events_[kMaxEvents];
  EpollFd epollFd_;
  EpollReactor(EpollFd &&fd);
  static EpollReactor* createPtr();
 public:
  static EpollReactor create();
  static std::unique_ptr<EpollReactor> createUnique();
  void addTask(Task *task);
  int runOnce();
  void loopForever();

  using duration_type = std::chrono::milliseconds;

  void runForDuration(duration_type minDuration);
  void stop();
  bool isRunning() const;
};




}} // aliens::reactor