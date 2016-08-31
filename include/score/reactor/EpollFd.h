#pragma once
#include "score/posix/FileDescriptor.h"

namespace score { namespace reactor {

class EpollFd {
 protected:
  posix::FileDescriptor fd_;
  EpollFd(posix::FileDescriptor &&descriptor);
 public:
  static EpollFd create();
  int get();
};

}} // score::reactor
