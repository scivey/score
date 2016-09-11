#include "score/posix/FileDescriptor.h"
#include "score/exceptions/exceptions.h"
#include "score/macros.h"

#include <atomic>
#include <glog/logging.h>
#include <fcntl.h>
#include <cstdio>

using namespace score::exceptions;

namespace score { namespace posix {

FileDescriptor::FileDescriptor(){}

FileDescriptor::FileDescriptor(int fd): fd_(fd){}

void FileDescriptor::maybeClose() {
  if (fd_ > 0) {
    VLOG(50) << "close(" << fd_ << ")";
    ::close(fd_);
    fd_ = 0;
  }
}
FileDescriptor FileDescriptor::fromIntExcept(int fd) {
  SCHECK(fd >= 0);
  return FileDescriptor(fd);
}
FileDescriptor::FileDescriptor(FileDescriptor &&other): fd_(other.fd_) {
  VLOG(50) << "move construction {fd=" << fd_ << "}";
  other.fd_ = 0;
}
FileDescriptor& FileDescriptor::operator=(FileDescriptor &&other) {
  int temp = other.fd_;
  other.fd_ = fd_;
  fd_ = temp;
  return *this;
}
FileDescriptor::~FileDescriptor() {
  maybeClose();
}

int FileDescriptor::getFdNo() const {
  return fd_;
}
bool FileDescriptor::valid() const {
  return fd_ > 0;
}

FileDescriptor::operator bool() const {
  return valid();
}
void FileDescriptor::makeNonBlocking() {
  SDCHECK(valid());
  int flags, status;
  flags = fcntl(fd_, F_GETFL, 0);
  if (flags == -1) {
    throw score::exceptions::SystemError(errno);
  }
  flags |= O_NONBLOCK;
  status = fcntl(fd_, F_SETFL, flags);
  SCORE_CHECK_SYSCALL(status);
}
void FileDescriptor::close() {
  SDCHECK(valid());
  SCORE_CHECK_SYSCALL(::close(fd_));
}

}} // score::posix
