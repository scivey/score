#include "aliens/reactor/FileDescriptor.h"
#include "aliens/exceptions/exceptions.h"
#include "aliens/exceptions/macros.h"

#include <glog/logging.h>
#include <fcntl.h>
#include <cstdio>

namespace aliens { namespace reactor {

FileDescriptor::FileDescriptor(){}

FileDescriptor::FileDescriptor(int fd): fd_(fd){}

void FileDescriptor::maybeClose() {
  if (fd_ > 0) {
    ::close(fd_);
    fd_ = 0;
  }
}
FileDescriptor FileDescriptor::fromIntExcept(int fd) {
  CHECK(fd >= 0);
  return FileDescriptor(fd);
}
FileDescriptor::FileDescriptor(FileDescriptor &&other): fd_(other.fd_) {
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
int FileDescriptor::get() {
  return fd_;
}
bool FileDescriptor::valid() const {
  return fd_ > 0;
}

FileDescriptor::operator bool() const {
  return valid();
}
void FileDescriptor::makeNonBlocking() {
  CHECK(valid());
  int flags, status;
  flags = fcntl(fd_, F_GETFL, 0);
  if (flags == -1) {
    throw aliens::exceptions::SystemError(errno);
  }
  flags |= O_NONBLOCK;
  status = fcntl(fd_, F_SETFL, flags);
  ALIENS_CHECK_SYSCALL(status);
}
void FileDescriptor::close() {
  CHECK(valid());
  ALIENS_CHECK_SYSCALL(::close(fd_));
}

}} // aliens::reactor
