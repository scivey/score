#pragma once
#include "aliens/reactor/TCPChannel.h"
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/SocketAddr.h"
#include "aliens/reactor/TCPConnectionInfo.h"

namespace aliens { namespace reactor {

class TCPClient {
 public:
  using channel_ptr = std::unique_ptr<TCPChannel>;
  using EventHandler = TCPChannel::EventHandler;
 protected:
  EpollReactor *reactor_ {nullptr};
  channel_ptr channel_;
  TCPClient(EpollReactor *reactor, channel_ptr);
 public:
  TCPChannel::EpollTask* getEpollTask();
  static TCPClient* connectPtr(EpollReactor*, EventHandler*, const SocketAddr&);
  static TCPClient connect(EpollReactor*, EventHandler*, const SocketAddr&);
};

}} // aliens::reactor
