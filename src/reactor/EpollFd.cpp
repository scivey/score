#include "aliens/reactor/EpollFd.h"
#include <memory>
#include <sys/epoll.h>
#include "aliens/macros.h"

using aliens::posix::FileDescriptor;

namespace aliens { namespace reactor {

EpollFd::EpollFd(FileDescriptor &&descriptor)
  : fd_(std::forward<FileDescriptor>(descriptor)) {}

EpollFd EpollFd::create() {
  int fd = epoll_create1(EPOLL_CLOEXEC);
  ALIENS_CHECK_SYSCALL(fd);
  return EpollFd(FileDescriptor::fromIntExcept(fd));
}

int EpollFd::get() {
  ADCHECK(!!fd_);
  return fd_.getFdNo();
}

}} // aliens::reactor
