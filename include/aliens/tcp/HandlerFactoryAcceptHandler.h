#pragma once
#include <boost/asio.hpp>
#include "aliens/tcp/TCPAcceptServer.h"
#include "aliens/tcp/TCPSocket.h"

namespace aliens { namespace tcp {

class HandlerFactoryAcceptHandler : public TCPAcceptServer::EventHandler {
  using HandlerFactory = TCPSocket::EventHandlerFactory;
  HandlerFactory* sessionHandlerFactory_ {nullptr};
 public:
  HandlerFactoryAcceptHandler(HandlerFactory *factory)
    : sessionHandlerFactory_(factory) {}
  void onAcceptSuccess(boost::asio::ip::tcp::socket&& sock) override {
    LOG(INFO) << "onAcceptSuccess!";
    // obviously this needs memory management.
    auto session = std::make_shared<TCPSocket>(
      std::move(sock), sessionHandlerFactory_->getHandler()
    );
    session->setDoneCallback([session]() {
      LOG(INFO) << "DONE CALLBACK!";
    });
    session->start();
  }
  void onAcceptError(boost::system::error_code ec) override {
    LOG(INFO) << "onAcceptError: " << ec;
  }
  void onStarted() override {
    LOG(INFO) << "onStarted!";
    startAccepting();
  }
};

}} // aliens::tcp
