#pragma once
#include "score/net/TCPChannel.h"
#include "score/reactor/EpollReactor.h"
#include "score/net/SocketAddr.h"
#include "score/net/TCPConnectionInfo.h"

namespace score { namespace net {

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

}} // score::net
