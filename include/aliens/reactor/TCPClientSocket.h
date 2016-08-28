#pragma once
#include <glog/logging.h>
#include <glog/logging.h>
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/reactor/TCPChannel.h"
#include "aliens/reactor/SocketAddr.h"
#include "aliens/reactor/ParentHaving.h"
#include "aliens/async/ErrBack.h"
#include "aliens/io/NonOwnedBufferPtr.h"

namespace aliens { namespace reactor {

class TCPClientSocket {
 public:
  using EventHandler = TCPChannel::EventHandler;
 protected:
  std::unique_ptr<TCPChannel> channel_;
  TCPClientSocket(std::unique_ptr<TCPChannel> channel);
 public:
  static TCPClientSocket connect(
    SocketAddr remoteAddr, EventHandler*
  );
  static TCPClientSocket* connectPtr(
    SocketAddr remoteAddr, EventHandler*
  );
  static std::shared_ptr<TCPClientSocket> connectShared(
    SocketAddr remoteAddr, EventHandler*
  );
};

}} // aliens::reactor