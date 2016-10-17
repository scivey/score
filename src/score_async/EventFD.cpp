#include "score_async/EventFD.h"
#include "score/util/try_helpers.h"
#include <glog/logging.h>
#include <sys/eventfd.h>

using score::posix::FileDescriptor;
using score::util::makeTryFailure;
using score::util::makeTrySuccess;
using score::util::doWithValue;

namespace evs { namespace events {

using fd_try_t = typename EventFD::fd_try_t;

EventFD::EventFD(FileDescriptor&& fd)
  : fd_(std::forward<FileDescriptor>(fd)){}

EventFD EventFD::create() {
  int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  CHECK(fd > 0);
  return EventFD(FileDescriptor::takeOwnership(fd));
}

fd_try_t EventFD::getFd() {
  return fd_.get();
}

Try<int64_t> EventFD::read() {
  return doWithValue(getFd(), [](int fd) {
    int64_t buff;
    ssize_t nr = ::read(fd, (void*) &buff, sizeof(buff));
    if (nr < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return makeTryFailure<int64_t, NotReady>(
          "[{}] NotReady", errno
        );
      } else {
        return makeTryFailure<int64_t, ReadError>(
          "ReadError: [{}] '{}'", errno, strerror(errno)
        );
      }
    } else if (nr == 0) {
      return makeTryFailure<int64_t, NotReady>(
        "[{}] NotReady", errno
      );
    }
    DCHECK(nr == 8);
    return makeTrySuccess<int64_t>(buff);
  });
}

Try<Unit> EventFD::write(int64_t num) {
  auto fd = getFd();
  return doWithValue(getFd(), [num](int fd) {
    int64_t buff = num;
    ssize_t nr = ::write(fd, (void*) &buff, sizeof(buff));
    if (nr < 0) {
      return makeTryFailure<Unit, WriteError>(
        "WriteError: [{}] '{}'", errno, strerror(errno)
      );
    } else if (nr != sizeof(buff)) {
      return makeTryFailure<Unit, WriteError>(
        "Unknown write error."
      );
    }
    return makeTrySuccess<Unit>();
  });
}

bool EventFD::good() const {
  return !!fd_;
}

EventFD::operator bool() const {
  return good();
}

}} // evs::events

