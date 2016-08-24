#include <thread>
#include <string>
#include <sstream>
#include <memory>
#include <folly/MoveWrapper.h>
#include <glog/logging.h>
#include <boost/asio.hpp>
#include "aliens/FixedBuffer.h"

using namespace std;
using boost::asio::ip::tcp;
using aliens::FixedBuffer;

namespace aliens { namespace io {


using Buffer = FixedBuffer<1024>;

template<typename TParent>
class EventHandlerBase {
 public:
  using TParent = parent_type

 protected:
  friend class TParent;
  TParent *parent_ {nullptr};

  void setParent(TParent *parent) {
    parent_ = parent;
  }

  TParent* getParent() const {
    return parent_;
  }
 public:
  EventHandlerBase(){}

  bool hasParent() const {
    return !!parent_;
  }
};


class TCPServerSession : public std::enable_shared_from_this<TCPServerSession> {
 public:

  using error_code = boost::system::error_code;

  class EventHandler: public EventHandlerBase<TCPServerSession> {
   public:
    operator bool() const {
      return hasParent();
    }

    virtual void read(std::unique_ptr<Buffer> buff) {
      CHECK(good());
      parent_->readInto(std::move(buff));
    }
    virtual void write(std::unique_ptr<Buffer> buff) {
      CHECK(good());
      parent_->triggerWrite(std::move(buff));
    }
    virtual void close() {
      CHECK(good());
      parent_->close();
    }

    virtual void beforeSessionDestroyed() {}

    virtual void onWriteSuccess(size_t nr) = 0;
    virtual void onWriteError(error_code ec, size_t nr) = 0;
    virtual void onConnected() = 0;
    virtual void onReadSuccess(std::unique_ptr<Buffer>) = 0;
    virtual void onReadError(error_code ec) = 0;
    virtual void onBeforeClose() {}
    virtual void onAfterClose() {}
  };

  friend class EventHandler;

  enum class Status {
    OPEN = 1, CLOSED = 2
  };

 protected:
  tcp::socket socket_;
  // Buffer buff_;
  EventHandler *handler_ {nullptr};
  Status status_ {Status::OPEN};

 public:
  TCPServerSession(tcp::socket &&socket, EventHandler *handler)
    : socket_(std::move(socket)), handler_(handler) {}

  void start() {
    handler_->setParent(this);
    handler_->onConnected();
  }

  ~TCPRequestHandler() {
    if (handler_) {
      handler_->beforeSessionDestroyed();
    }
    LOG(INFO) << "~TCPRequestHandler()";
  }
 protected:
  void readInto(std::unique_ptr<Buffer> buff) {
    auto self(shared_from_this());
    boost::asio::buffer asioBuff(buff->body(), buff->capacity());
    auto wrapped = folly::makeMoveWrapper(buff);
    socket_.async_read_some(asioBuff,
      [this, self, wrapped](error_code ec, std::size_t nr) {
        folly::MoveWrapper<std::unique_ptr<Buffer>> movedBuff = wrapped;
        std::unique_ptr<Buffer> buff = wrapped.move();
        if (ec) {
          handler_->onReadFailed(ec);
        } else {
          handler_->onReadSucceeded(std::move(buff));
        }
      }
    );
  }
  void triggerWrite(std::unique_ptr<Buffer> buff) {
    using MoveWrapper = folly::MoveWrapper<decltype(buff)>;
    auto self(shared_from_this());
    boost::asio::buffer asioBuff(buff->body(), buff->capacity());
    auto wrapped = folly::makeMoveWrapper(buff);
    boost::asio::async_write(socket_, asioBuff,
      [this, self, wrapped](error_code ec, std::size_t nr) {
        MoveWrapper movedBuff = wrapped;
        std::unique_ptr<Buffer> buff = wrapped.move();
        if (ec) {
          handler_->onWriteFailed(ec, nr);
        } else {
          handler_->onWriteSucceeded(nr);
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


class TCPAcceptServer: public std::enable_shared_from_this<TCPAcceptServer> {
 public:

  using boost::system::error_code;
  class EventHandler : public EventHandlerBase<TCPAcceptServer> {
   public:
    virtual void onAcceptSuccess(tcp::socket&&) = 0;
    virtual void onAcceptError(error_code ec) = 0;
    virtual void onStarted() = 0;
    virtual void onStopRequested() = 0;

    virtual void onBeforeClose() {}
    virtual void onAfterClose() {}
    virtual void close() {
      getParent()->close();
    }

  };

  friend class EventHandler;

  enum class Status {
    OPEN = 1, CLOSED = 2
  };

 protected:
  tcp::acceptor acceptor_;
  EventHandler *handler_ {nullptr};
  tcp::socket socket_;
  Status status_ {Status::CLOSED};

  TCPAcceptServer(const TCPAcceptServer&) = delete;
  TCPAcceptServer& operator=(const TCPAcceptServer&) = delete;

 public:
  TCPAcceptServer(boost::asio::io_service &ioService,
      EventHandler *handler, const tcp::endpoint &tcpEndpoint)
    : acceptor_(ioService, tcpEndpoint), handler_(handler), socket_(ioService) {}

  TCPAcceptServer(TCPAcceptServer&&) = default;
  TCPAcceptServer& operator=(TCPAcceptServer&&) = default;

  void start() {
    CHECK(status_ == status_::CLOSED);
    CHECK(!!handler_);
    handler_->onStarted();
  }

 protected:
  void close() {
    CHECK(status_ == Status::OPEN);
    if (handler_) {
      handler_->onBeforeClose();
    }
    acceptor_.stop();
    status_ = status_::CLOSED;
    if (handler_) {
      handler_->onAfterClose();
    }
  }

  void startAccepting() {
    CHECK(status_ == Status::CLOSED);
    status_ = Status::OPEN;
    doAccept();
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
    maybeClose();
  }
};


template<typename THandler>
class SessionHandlerFactory {
 public:
  virtual THandler* create() = 0;
};


int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start";
  short portNo = 5097;
  thread serverThread([portNo]() {
    try {
      boost::asio::io_service ioService;
      auto server = std::make_shared<TCPServer>(ioService, tcp::endpoint(tcp::v4(), portNo));
      server->listen();
      ioService.run();
    } catch (std::exception &ex) {
      LOG(INFO) << "err! " << ex.what();
    }
  });
  serverThread.join();
  LOG(INFO) << "end";
}

}} // aliens::io
