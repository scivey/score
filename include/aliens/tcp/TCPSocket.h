#pragma once
#include <memory>
#include <glog/logging.h>
#include <boost/asio.hpp>
#include <folly/MoveWrapper.h>
#include "aliens/FixedBuffer.h"
#include "aliens/async/EventHandlerBase.h"
#include "aliens/async/VoidCallback.h"
#include "aliens/Maybe.h"
#include "aliens/io/SocketAddr.h"
#include "aliens/async/IOService.h"

namespace aliens { namespace tcp {

class TCPSocket : public std::enable_shared_from_this<TCPSocket> {
 public:

  using error_code = boost::system::error_code;
  using asio_tcp = boost::asio::ip::tcp;

  class EventHandler: public async::EventHandlerBase<TCPSocket> {
   public:
    operator bool() const {
      return hasParent();
    }
    virtual void read(std::unique_ptr<Buffer> buff) {
      CHECK(hasParent());
      parent_->readInto(std::move(buff));
    }
    virtual void write(std::unique_ptr<Buffer> buff) {
      CHECK(hasParent());
      parent_->triggerWrite(std::move(buff));
    }
    virtual void close() {
      CHECK(hasParent());
      parent_->close();
    }

    virtual void onBeforeClose() {}
    virtual void onAfterClose() {}

    virtual void onWriteSuccess(size_t nr) = 0;
    virtual void onWriteError(error_code ec, size_t nr) = 0;
    virtual void onConnectSuccess() = 0;
    virtual void onConnectError(error_code ec) = 0;
    virtual void onReadSuccess(std::unique_ptr<Buffer>) = 0;
    virtual void onReadError(error_code ec) = 0;
  };

  friend class EventHandler;

  class EventHandlerFactory {
   public:
    virtual EventHandler* getHandler() = 0;
  };

  enum class Status {
    OPEN = 1, CLOSED = 2
  };

 protected:
  asio_tcp::socket socket_;
  EventHandler *handler_ {nullptr};
  Status status_ {Status::OPEN};
  Maybe<async::VoidCallback> onFinished_;
 public:
  TCPSocket(asio_tcp::socket &&socket, EventHandler *handler)
    : socket_(std::move(socket)), handler_(handler) {}

  TCPSocket(async::IOService *ioService, EventHandler *handler)
    : socket_(ioService->getBoostService()), handler_(handler) {}

  void start() {
    handler_->setParent(this);
    handler_->onConnectSuccess();
  }

  ~TCPSocket() {
    LOG(INFO) << "~TCPSocket()";
  }

  template<typename TCallable>
  void setDoneCallback(TCallable &&callable) {
    CHECK(!onFinished_.hasValue());
    onFinished_.assign(std::move(callable));
  }

  template<typename TCallable>
  void setDoneCallback(const TCallable &callable) {
    CHECK(!onFinished_.hasValue());
    onFinished_.assign(callable);
  }

 protected:
  void readInto(std::unique_ptr<Buffer> buff) {
    auto self(shared_from_this());
    auto asioBuff = boost::asio::buffer(buff->body(), buff->capacity());
    auto wrapped = folly::makeMoveWrapper(buff);
    socket_.async_read_some(asioBuff,
      [this, self, wrapped](error_code ec, std::size_t nr) {
        folly::MoveWrapper<std::unique_ptr<Buffer>> movedBuff = wrapped;
        std::unique_ptr<Buffer> buff = movedBuff.move();
        if (ec) {
          handler_->onReadError(ec);
        } else {
          handler_->onReadSuccess(std::move(buff));
        }
      }
    );
  }
  void triggerWrite(std::unique_ptr<Buffer> buff) {
    using MoveWrapper = folly::MoveWrapper<decltype(buff)>;
    auto self(shared_from_this());
    auto asioBuff = boost::asio::buffer(buff->body(), buff->capacity());
    auto wrapped = folly::makeMoveWrapper(buff);
    boost::asio::async_write(socket_, asioBuff,
      [this, self, wrapped](error_code ec, std::size_t nr) {
        MoveWrapper movedBuff = wrapped;
        std::unique_ptr<Buffer> buff = movedBuff.move();
        if (ec) {
          handler_->onWriteError(ec, nr);
        } else {
          handler_->onWriteSuccess(nr);
        }
      }
    );
  }
  void close() {
    CHECK(status_ == Status::OPEN);
    if (handler_) {
      handler_->onBeforeClose();
    }
    socket_.close();
    status_ = Status::CLOSED;
    if (handler_) {
      handler_->onAfterClose();
    }
    if (onFinished_.hasValue()) {
      onFinished_.value().invoke();
    }
  }

 protected:
  void doConnect(async::IOService *service, const io::SocketAddr &addr) {
    asio_tcp::resolver resolver(service->getBoostService());
    asio_tcp::resolver::iterator endpointIter = resolver.resolve({addr.host.c_str(), "5017"});
    auto self = shared_from_this();
    boost::asio::async_connect(socket_, endpointIter,
      [this, self](boost::system::error_code ec, asio_tcp::resolver::iterator) {
        if (ec) {
          handler_->onConnectError(ec);
        } else {
          handler_->onConnectSuccess();
        }
      }
    );
  }

 public:
  static std::shared_ptr<TCPSocket> connect(async::IOService* service,
      EventHandler *handler, const io::SocketAddr &addr) {
    auto result = std::make_shared<TCPSocket>(service, handler);
    handler->setParent(result.get());
    result->doConnect(service, addr);
    return result;
  }
};


}} // aliens::tcp