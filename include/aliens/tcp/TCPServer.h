#pragma once
#include "aliens/tcp/TCPAcceptServer.h"
#include "aliens/tcp/TCPServerSession.h"
#include "aliens/tcp/SessionHandlerFactoryAcceptHandler.h"
#include "aliens/async/IOService.h"
#include <memory>
namespace aliens { namespace tcp {

class TCPServer {
 public:
  using SessionHandler = TCPServerSession::EventHandler;
  using SessionHandlerFactory = TCPServerSession::EventHandlerFactory;
  using AcceptHandler = TCPAcceptServer::EventHandler;
 protected:
  async::IOService *ioService_ {nullptr};
  std::shared_ptr<SessionHandlerFactory> sessionHandlerFactory_ {nullptr};
  std::shared_ptr<TCPAcceptServer> acceptServer_ {nullptr};
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