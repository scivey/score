#pragma once
#include <boost/asio.hpp>
#include "aliens/tcp/TCPAcceptServer.h"
#include "aliens/tcp/TCPServerSession.h"

namespace aliens { namespace tcp {


class SessionHandlerFactoryAcceptHandler : public TCPAcceptServer::EventHandler {
  using HandlerFactory = TCPServerSession::EventHandlerFactory;
  HandlerFactory* sessionHandlerFactory_ {nullptr};
 public:
  SessionHandlerFactoryAcceptHandler(HandlerFactory *factory)
    : sessionHandlerFactory_(factory) {}
  void onAcceptSuccess(boost::asio::ip::tcp::socket&& sock) override {
    LOG(INFO) << "onAcceptSuccess!";
    // obviously this needs memory management.
    auto session = std::make_shared<TCPServerSession>(
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
