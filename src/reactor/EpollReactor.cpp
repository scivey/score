#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/EpollFd.h"
#include "aliens/exceptions/macros.h"
#include <memory>
#include <glog/logging.h>
#include <sys/epoll.h>
#include <sys/types.h>


namespace aliens { namespace reactor {

using duration_type = typename EpollReactor::duration_type;

bool epollEventHasError(const epoll_event *evt) {
  return evt->events & EPOLLERR
    || evt->events & EPOLLHUP
    || (!evt->events & EPOLLIN);
}

bool epollEventHasError(const epoll_event &evt) {
  return epollEventHasError(&evt);
}

EpollReactor* EpollReactor::Task::getReactor() const {
  return reactor_;
}
void EpollReactor::Task::setReactor(EpollReactor *reactor) {
  reactor_ = reactor;
}

EpollReactor::EpollReactor(EpollFd &&fd, const EpollReactor::Options &opts)
  : epollFd_(std::forward<EpollFd>(fd)),
    options_(opts) {
  memset((void*) &events_, '\0', sizeof(events_));
}

EpollReactor* EpollReactor::createPtr(const EpollReactor::Options &opts) {
  return new EpollReactor(EpollFd::create(), opts);
}

EpollReactor EpollReactor::create(const EpollReactor::Options &opts) {
  return EpollReactor(EpollFd::create(), opts);
}

std::unique_ptr<EpollReactor> EpollReactor::createUnique(const EpollReactor::Options &ops) {
  return std::unique_ptr<EpollReactor>(createPtr(ops));
}

void EpollReactor::addTask(Task *task) {
  epoll_event evt;
  memset(&evt, 0, sizeof(evt));
  evt.data.ptr = (void*) task;
  evt.events = EPOLLIN | EPOLLOUT | EPOLLET;
  ALIENS_CHECK_SYSCALL(epoll_ctl(
    epollFd_.get(), EPOLL_CTL_ADD, task->getFd(), &evt
  ));
  task->setReactor(this);
  tasks_.push_back(task);
}

int EpollReactor::runOnce() {
  size_t timeoutMsec = options_.getWaitTimeout().count();
  int nEvents = epoll_wait(epollFd_.get(), events_, kMaxEvents, timeoutMsec);
  for (size_t i = 0; i < nEvents; i++) {
    auto task = (Task*) events_[i].data.ptr;
    if (epollEventHasError(events_[i])) {
      task->onError();
      continue;
    }
    if (events_[i].events & EPOLLIN) {
      task->onReadable();
    }
    if (events_[i].events & EPOLLOUT) {
      task->onWritable();
    }
  }
  return nEvents;
}

void EpollReactor::loopForever() {
  running_ = true;
  for (;;) {
    runOnce();
    if (!running_) {
      break;
    }
  }
}

void EpollReactor::runForDuration(duration_type minDuration) {
  auto start = std::chrono::system_clock::now().time_since_epoch();
  for (;;) {
    runOnce();
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed = now - start;
    if (elapsed >= minDuration) {
      break;
    }
  }
}

void EpollReactor::stop() {
  CHECK(running_);
  running_ = false;
}

bool EpollReactor::isRunning() const {
  return running_;
}

void EpollReactor::Options::setWaitTimeout(duration_type ms) {
  waitTimeout_ = ms;
}

duration_type EpollReactor::Options::getWaitTimeout() const {
  return waitTimeout_;
}

}} // aliens::reactor
