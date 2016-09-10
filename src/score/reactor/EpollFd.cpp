#include "score/reactor/EpollFd.h"
#include <memory>
#include <sys/epoll.h>
#include "score/macros.h"

using score::posix::FileDescriptor;

namespace score { namespace reactor {

EpollFd::EpollFd(FileDescriptor &&descriptor)
  : fd_(std::forward<FileDescriptor>(descriptor)) {}

EpollFd EpollFd::create() {
  int fd = epoll_create1(EPOLL_CLOEXEC);
  SCORE_CHECK_SYSCALL(fd);
  return EpollFd(FileDescriptor::fromIntExcept(fd));
}

int EpollFd::get() {
  SDCHECK(!!fd_);
  return fd_.getFdNo();
}

}} // score::reactor
