#pragma once

#include "score/posix/FileDescriptor.h"
#include "score/exceptions/ScoreError.h"
#include "score/macros.h"
#include "score/Try.h"
#include "score/Unit.h"

#include <utility>
#include <type_traits>

namespace score { namespace async {

class EventFD {
 protected:
  posix::FileDescriptor fd_;
  EventFD(posix::FileDescriptor&&);
 public:
  SCORE_DECLARE_EXCEPTION(EventFDError, ScoreError);
  SCORE_DECLARE_EXCEPTION(Invalid, EventFDError);
  SCORE_DECLARE_EXCEPTION(NotReady, EventFDError);
  SCORE_DECLARE_EXCEPTION(IOError, EventFDError);
  SCORE_DECLARE_EXCEPTION(ReadError, IOError);
  SCORE_DECLARE_EXCEPTION(WriteError, IOError);
  static EventFD create();
  using fd_try_t = decltype(std::declval<posix::FileDescriptor>().get());
  fd_try_t getFd();
  Try<int64_t> read();
  Try<Unit> write(int64_t num);
  bool good() const;
  operator bool() const;
};


}} // evs::events

