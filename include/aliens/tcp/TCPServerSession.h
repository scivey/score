#pragma once
#include <memory>
#include <glog/logging.h>
#include <boost/asio.hpp>
#include <folly/MoveWrapper.h>
#include "aliens/FixedBuffer.h"
#include "aliens/async/EventHandlerBase.h"

namespace aliens { namespace tcp {

class TCPServerSession : public std::enable_shared_from_this<TCPServerSession> {
 public:

  using error_code = boost::system::error_code;
  using asio_tcp = boost::asio::ip::tcp;

  class EventHandler: public async::EventHandlerBase<TCPServerSession> {
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

    virtual void beforeSessionDestroyed() {}
    virtual void onBeforeClose() {}
    virtual void onAfterClose() {}

    virtual void onWriteSuccess(size_t nr) = 0;
    virtual void onWriteError(error_code ec, size_t nr) = 0;
    virtual void onConnected() = 0;
    virtual void onReadSuccess(std::unique_ptr<Buffer>) = 0;
    virtual void onReadError(error_code ec) = 0;
  };

  friend class EventHandler;

  enum class Status {
    OPEN = 1, CLOSED = 2
  };

 protected:
  asio_tcp::socket socket_;
  // Buffer buff_;
  EventHandler *handler_ {nullptr};
  Status status_ {Status::OPEN};

 public:
  TCPServerSession(asio_tcp::socket &&socket, EventHandler *handler)
    : socket_(std::move(socket)), handler_(handler) {}

  void start() {
    handler_->setParent(this);
    handler_->onConnected();
  }

  ~TCPServerSession() {
    LOG(INFO) << "~TCPServerSession()";
    if (handler_) {
      handler_->beforeSessionDestroyed();
    }
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
  }
};


}} // aliens::tcp