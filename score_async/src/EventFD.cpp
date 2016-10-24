#include "score/async/EventFD.h"
#include "score/util/try_helpers.h"
#include <glog/logging.h>
#include <sys/eventfd.h>

using score::posix::FileDescriptor;
using score::util::makeTryFailure;
using score::util::makeTryFailureFromErrno;
using score::util::makeTrySuccess;
using score::util::doWithValue;

namespace score { namespace async {

EventFD::EventFD(FileDescriptor&& fd)
  : fd_(std::forward<FileDescriptor>(fd)){}

Try<EventFD> EventFD::create() {
  int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  if (fd <= 0) {
    return makeTryFailureFromErrno<EventFD, CouldntCreate>(
      errno, "Couldn't create EventFD"
    );
  }
  return makeTrySuccess<EventFD>(EventFD{FileDescriptor::takeOwnership(fd)});
}

Try<int> EventFD::getFDNum() {
  auto result = fd_.get();
  if (result.hasException<FileDescriptor::Invalid>()) {
    return makeTryFailure<int, EventFD::Invalid>(
      "Invalid EventFD: wrapping '{}'", result.exception().what()
    );
  }
  return result;
}

static const std::string kEventFDNotReadyMsg {"EventFD not ready"};

Try<int64_t> EventFD::read() {
  return doWithValue(getFDNum(), [](int fd) {
    int64_t buff;
    errno = 0;
    ssize_t nr = ::read(fd, (void*) &buff, sizeof(buff));
    if (nr < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return makeTryFailure<int64_t, NotReady>(kEventFDNotReadyMsg);
      } else {
        return makeTryFailureFromErrno<int64_t, ReadError>(
          errno, "EventFD read failed"
        );
      }
    } else if (nr == 0) {
      if (errno == 0) {
        return makeTryFailure<int64_t, NotReady>(kEventFDNotReadyMsg);
      } else {
        return makeTryFailureFromErrno<int64_t, NotReady>(
          errno, kEventFDNotReadyMsg
        );
      }
    }
    DCHECK(nr == 8);
    return makeTrySuccess<int64_t>(buff);
  });
}

Try<Unit> EventFD::write(int64_t num) {
  return doWithValue(getFDNum(), [num](int fd) {
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

}} // score::async

