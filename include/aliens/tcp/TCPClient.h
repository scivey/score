#pragma once

#include "aliens/async/IOService.h"
#include <boost/asio.hpp>

namespace aliens { namespace tcp {

class TCPClient {
 public:

 protected:
  async::IOService *ioService_ {nullptr};
  EventHandler *handler_;

 public:
  TCPServer(async::IOService *ioService, std::shared_ptr<SessionHandlerFactory> sessionFact)
    : ioService_(ioService), sessionHandlerFactory_(sessionFact) {}
  void listen(short portNo) {
    auto handler = new SessionHandlerFactoryAcceptHandler(sessionHandlerFactory_.get());
    acceptServer_.reset(new TCPAcceptServer(
      ioService_, handler, boost::asio::ip::tcp::endpoint(
        boost::asio::ip::tcp::v4(), portNo
      )
    ));
    acceptServer_->start();
  }

};

}} // aliens::tcp