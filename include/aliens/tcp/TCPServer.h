#pragma once
#include "aliens/tcp/TCPAcceptServer.h"
#include "aliens/tcp/TCPSocket.h"
#include "aliens/tcp/HandlerFactoryAcceptHandler.h"
#include "aliens/async/IOService.h"
#include <memory>
namespace aliens { namespace tcp {

class TCPServer {
 public:
  using SessionHandler = TCPSocket::EventHandler;
  using HandlerFactory = TCPSocket::EventHandlerFactory;
  using AcceptHandler = TCPAcceptServer::EventHandler;
 protected:
  async::IOService *ioService_ {nullptr};
  std::shared_ptr<HandlerFactory> sessionHandlerFactory_ {nullptr};
  std::shared_ptr<TCPAcceptServer> acceptServer_ {nullptr};
 public:
  TCPServer(async::IOService *ioService, std::shared_ptr<HandlerFactory> sessionFact)
    : ioService_(ioService), sessionHandlerFactory_(sessionFact) {}
  void listen(short portNo) {
    auto handler = new HandlerFactoryAcceptHandler(sessionHandlerFactory_.get());
    acceptServer_.reset(new TCPAcceptServer(
      ioService_, handler, boost::asio::ip::tcp::endpoint(
        boost::asio::ip::tcp::v4(), portNo
      )
    ));
    acceptServer_->start();
  }

};

}} // aliens::tcp