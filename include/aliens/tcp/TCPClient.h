#pragma once

#include <memory>
#include <boost/asio.hpp>
#include "aliens/async/IOService.h"
#include "aliens/FixedBuffer.h"
#include "aliens/tcp/TCPSocket.h"
#include "aliens/io/SocketAddr.h"

namespace aliens { namespace tcp {

class TCPClient: public std::enable_shared_from_this<TCPClient> {
 public:
  using EventHandler = TCPSocket::EventHandler;
 protected:
  using asio_tcp = boost::asio::ip::tcp;
  async::IOService *ioService_ {nullptr};
  EventHandler *handler_ {nullptr};
  io::SocketAddr socketAddr_;
  std::shared_ptr<TCPSocket> socket_ {nullptr};
 public:
  TCPClient(async::IOService* ioService, EventHandler *handler, const io::SocketAddr &socketAddr)
    : ioService_(ioService), handler_(handler), socketAddr_(socketAddr) {}
  void start() {
    socket_ = TCPSocket::connect(ioService_, handler_, socketAddr_);
  }
};

}} // aliens::tcp