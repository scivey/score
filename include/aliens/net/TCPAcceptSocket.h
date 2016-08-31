#pragma once
#include <glog/logging.h>
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/ParentHaving.h"

namespace aliens { namespace net {

class TCPAcceptSocket : public reactor::FdHandlerBase<TCPAcceptSocket> {
 public:
  class EventHandler: public ParentHaving<TCPAcceptSocket> {
   public:
    virtual void onAcceptSuccess(int inFd, const char *host, const char *port) = 0;
    virtual void onAcceptError(int err) = 0;
    virtual ~EventHandler() = default;
  };
 protected:
  EventHandler *handler_ {nullptr};
  TCPAcceptSocket(posix::FileDescriptor &&, EventHandler*);
 public:
  void onReadable();
  void onWritable();
  void onError();
  void listen();
  static TCPAcceptSocket bindPort(short portNo, EventHandler*);
};

}} // aliens::reactor