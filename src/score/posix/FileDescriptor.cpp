#include "score/posix/FileDescriptor.h"
#include <folly/Format.h>
#include <glog/logging.h>

namespace score { namespace posix {

FileDescriptor::FileDescriptor(int fd): fd_(fd){}

FileDescriptor::FileDescriptor(){}

FileDescriptor::FileDescriptor(FileDescriptor&& other): fd_(other.fd_) {
  other.fd_ = 0;
}

bool FileDescriptor::good() const {
  return fd_ > 0;
}

FileDescriptor::operator bool() const {
  return good();
}

Try<int> FileDescriptor::get() {
  if (good()) {
    return Try<int> { fd_ };
  }
  return Try<int> {
    folly::make_exception_wrapper<FileDescriptor::Invalid>(
      folly::sformat("Invalid file descriptor: {}", fd_)
    )
  };
}

Try<int> FileDescriptor::release() {
  auto result = get();
  if (good()) {
    fd_ = 0;
  }
  return result;
}


FileDescriptor& FileDescriptor::operator=(FileDescriptor&& other) {
  std::swap(fd_, other.fd_);
  return *this;
}

void FileDescriptor::maybeClose() {
  if (fd_ > 0) {
    int rc = close(fd_);
    if (rc < 0) {
      LOG(INFO) << "error on close? : '" << strerror(errno) << "'";
    }
    fd_ = 0;
  }
}

FileDescriptor::~FileDescriptor() {
  maybeClose();
}

FileDescriptor FileDescriptor::takeOwnership(int fd) {
  return FileDescriptor(fd);
}

}} // score::posix
