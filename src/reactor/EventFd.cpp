#include "aliens/reactor/EventFd.h"
#include "aliens/exceptions/macros.h"
#include <glog/logging.h>

namespace aliens { namespace reactor {

EventFd::EventFd(FileDescriptor &&desc, EventHandler *handler)
  : FdHandlerBase<EventFd>(std::forward<FileDescriptor>(desc)),
    handler_(handler) {}

void EventFd::stop() {
  throw exceptions::BaseError("not implemented.");
}

EventFd EventFd::create(EventFd::EventHandler *handler) {
  int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  ALIENS_CHECK_SYSCALL2(fd, "eventfd()");
  auto desc = FileDescriptor::fromIntExcept(fd);
  return EventFd(std::move(desc), handler);
}

std::shared_ptr<EventFd> EventFd::createShared(
    EventFd::EventHandler *handler) {
  return std::shared_ptr<EventFd>(new EventFd(create(
    handler
  )));
}


void EventFd::onWritable() {
  LOG(INFO) << "EventFd::onWritable()";
}

void EventFd::onError() {
  LOG(INFO) << "EventFd::onError()";
}

void EventFd::onReadable() {
  CHECK(!!handler_);
  uint64_t eventNo;
  CHECK(8 == read(getFdNo(), &eventNo, sizeof(eventNo)));
  handler_->onEvent(eventNo);
}

void EventFd::write(uint64_t msg) {
  CHECK(8 == ::write(getFdNo(), (void*) &msg, sizeof(msg)));
}


}} // aliens::reactor