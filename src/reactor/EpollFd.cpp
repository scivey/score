#include "aliens/reactor/EpollFd.h"
#include <memory>
#include <sys/epoll.h>
#include <glog/logging.h>
#include "aliens/exceptions/macros.h"

namespace aliens { namespace reactor {

EpollFd::EpollFd(FileDescriptor &&descriptor)
  : fd_(std::forward<FileDescriptor>(descriptor)) {}

EpollFd EpollFd::create() {
  int fd = epoll_create1(EPOLL_CLOEXEC);
  ALIENS_CHECK_SYSCALL(fd);
  return EpollFd(FileDescriptor::fromIntExcept(fd));
}

int EpollFd::get() {
  CHECK(!!fd_);
  return fd_.get();
}

}} // aliens::reactor
