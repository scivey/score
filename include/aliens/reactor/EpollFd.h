#pragma once
#include "aliens/posix/FileDescriptor.h"

namespace aliens { namespace reactor {

class EpollFd {
 protected:
  posix::FileDescriptor fd_;
  EpollFd(posix::FileDescriptor &&descriptor);
 public:
  static EpollFd create();
  int get();
};

}} // aliens::reactor
