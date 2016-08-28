#pragma once
#include <glog/logging.h>
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/reactor/SocketAddr.h"
#include "aliens/reactor/ParentHaving.h"
#include "aliens/reactor/TCPChannel.h"
#include "aliens/async/ErrBack.h"
#include "aliens/io/NonOwnedBufferPtr.h"

namespace aliens { namespace reactor {

class TCPServerSocket {
 public:
  using EventHandler = TCPChannel::EventHandler;
  friend class EventHandler;
 protected:
  std::unique_ptr<TCPChannel> channel_;
  TCPServerSocket(std::unique_ptr<TCPChannel> channel);
 public:
  static TCPServerSocket fromAccepted(
    std::unique_ptr<TCPChannel>
  );
  static TCPServerSocket* fromAcceptedPtr(
    std::unique_ptr<TCPChannel>
  );
  static std::shared_ptr<TCPServerSocket> fromAcceptedShared(
    std::unique_ptr<TCPChannel>
  );
};

}} // aliens::reactor