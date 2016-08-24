#pragma once
#include <memory>
#include <glog/logging.h>
#include <boost/asio.hpp>
#include "aliens/async/EventHandlerBase.h"
#include "aliens/async/IOService.h"



namespace aliens { namespace tcp {


class TCPAcceptServer: public std::enable_shared_from_this<TCPAcceptServer> {
 public:

  using error_code = boost::system::error_code;
  using asio_tcp = boost::asio::ip::tcp;
  class EventHandler : public async::EventHandlerBase<TCPAcceptServer> {
   public:
    virtual void onAcceptSuccess(asio_tcp::socket&&) = 0;
    virtual void onAcceptError(error_code ec) = 0;
    virtual void onStarted() = 0;

    virtual void onBeforeClose() {}
    virtual void onAfterClose() {}

    virtual void onStopRequested() {
      getParent()->stop();
    }

    virtual void close() {
      getParent()->close();
    }

    virtual void startAccepting() {
      CHECK(hasParent());
      getParent()->startAccepting();
    }

  };

  friend class EventHandler;

  enum class Status {
    OPEN = 1, CLOSED = 2
  };

 protected:
  async::IOService *ioService_ {nullptr};
  asio_tcp::acceptor acceptor_;
  EventHandler *handler_ {nullptr};
  asio_tcp::socket socket_;
  Status status_ {Status::CLOSED};

  TCPAcceptServer(const TCPAcceptServer&) = delete;
  TCPAcceptServer& operator=(const TCPAcceptServer&) = delete;

 public:
  TCPAcceptServer(async::IOService *ioService,
      EventHandler *handler, const asio_tcp::endpoint &tcpEndpoint)
    : ioService_(ioService),
      acceptor_(ioService->getBoostService(), tcpEndpoint),
      handler_(handler),
      socket_(ioService->getBoostService()) {}

  TCPAcceptServer(TCPAcceptServer&&) = default;
  TCPAcceptServer& operator=(TCPAcceptServer&&) = default;

  void start() {
    CHECK(status_ == Status::CLOSED);
    CHECK(!!handler_);
    handler_->setParent(this);
    handler_->onStarted();
  }

 protected:
  void close() {
    CHECK(status_ == Status::OPEN);
    if (handler_) {
      handler_->onBeforeClose();
    }
    LOG(INFO) << "FIXME: not actually stopping acceptor!";
    // acceptor_.stop();
    status_ = Status::CLOSED;
    if (handler_) {
      handler_->onAfterClose();
    }
  }

  void startAccepting() {
    LOG(INFO) << "startAccepting : 0";
    CHECK(status_ == Status::CLOSED);
    status_ = Status::OPEN;
    LOG(INFO) << "startAccepting : 1";
    doAccept();
    LOG(INFO) << "startAccepting : 2";
  }

  void doAccept() {
    auto self = shared_from_this();
    acceptor_.async_accept(socket_,
      [self, this](error_code ec) {
        if (ec) {
          handler_->onAcceptError(ec);
        } else {
          handler_->onAcceptSuccess(std::move(socket_));
        }
        if (status_ == Status::OPEN) {
          doAccept();
        }
      }
    );
  }

  void maybeClose() {
    if (status_ == Status::OPEN) {
      close();
    }
  }

 public:
  void stop() {
    if (handler_) {
      handler_->onStopRequested();
    } else {
      CHECK(status_ == Status::CLOSED);
    }
  }

  ~TCPAcceptServer() {
    LOG(INFO) << "~TCPAcceptServer()";
    maybeClose();
  }
};


}} // aliens::tcp