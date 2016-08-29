#include "aliens/reactor/EventFd.h"
#include "aliens/macros.h"
#include <glog/logging.h>

using aliens::posix::FileDescriptor;
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

EventFd* EventFd::createPtr(
    EventFd::EventHandler *handler) {
  return new EventFd(create(
    handler
  ));
}


void EventFd::onWritable() {
  VLOG(50) << "EventFd::onWritable()";
}

void EventFd::onError() {
  VLOG(50) << "EventFd::onError()";
}

void EventFd::onReadable() {
  ADCHECK(!!handler_);
  uint64_t eventNo;
  ADCHECK(8 == read(getFdNo(), &eventNo, sizeof(eventNo)));
  handler_->onEvent(eventNo);
}

void EventFd::write(uint64_t msg) {
  ADCHECK(8 == ::write(getFdNo(), (void*) &msg, sizeof(msg)));
}


}} // aliens::reactor