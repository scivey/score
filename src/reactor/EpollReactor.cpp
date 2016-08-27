#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/EpollFd.h"
#include "aliens/exceptions/macros.h"
#include <memory>
#include <glog/logging.h>
#include <sys/epoll.h>
#include <sys/types.h>


namespace aliens { namespace reactor {

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

EpollReactor::EpollReactor(EpollFd &&fd)
  : epollFd_(std::forward<EpollFd>(fd)) {
  memset((void*) &events_, '\0', sizeof(events_));
}

EpollReactor* EpollReactor::createPtr() {
  return new EpollReactor(EpollFd::create());
}

EpollReactor EpollReactor::create() {
  return EpollReactor(EpollFd::create());
}

std::unique_ptr<EpollReactor> EpollReactor::createUnique() {
  return std::unique_ptr<EpollReactor>(createPtr());
}

void EpollReactor::addTask(Task *task) {
  epoll_event evt;
  evt.data.ptr = (void*) task;
  evt.events = EPOLLIN | EPOLLOUT | EPOLLET;
  ALIENS_CHECK_SYSCALL(epoll_ctl(
    epollFd_.get(), EPOLL_CTL_ADD, task->getFd(), &evt
  ));
  task->setReactor(this);
  tasks_.push_back(task);
}

int EpollReactor::runOnce() {
  int nEvents = epoll_wait(epollFd_.get(), events_, kMaxEvents, 20);
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

using duration_type = typename EpollReactor::duration_type;
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

}} // aliens::reactor
