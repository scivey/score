#pragma once
#include "aliens/reactor/FileDescriptor.h"
namespace aliens { namespace reactor {

class EpollFd {
 protected:
  FileDescriptor fd_;
  EpollFd(FileDescriptor &&descriptor);
 public:
  static EpollFd create();
  int get();
};

}} // aliens::reactor
