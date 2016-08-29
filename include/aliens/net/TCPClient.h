#pragma once
#include "aliens/net/TCPChannel.h"
#include "aliens/reactor/EpollReactor.h"
#include "aliens/net/SocketAddr.h"
#include "aliens/net/TCPConnectionInfo.h"

namespace aliens { namespace net {

class TCPClient {
 public:
  using channel_ptr = std::unique_ptr<TCPChannel>;
  using EventHandler = TCPChannel::EventHandler;
 protected:
  reactor::EpollReactor *reactor_ {nullptr};
  channel_ptr channel_;
  TCPClient(reactor::EpollReactor *reactor, channel_ptr);
 public:
  TCPChannel::EpollTask* getEpollTask();
  static TCPClient* connectPtr(
    reactor::EpollReactor*,
    EventHandler*,
    const net::SocketAddr&);
  static TCPClient connect(
    reactor::EpollReactor*,
    EventHandler*,
    const net::SocketAddr&);
};

}} // aliens::net
