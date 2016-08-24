#include <thread>
#include <string>
#include <sstream>
#include <atomic>

#include <memory>
#include <folly/MoveWrapper.h>
#include <glog/logging.h>
#include <boost/asio.hpp>
#include "aliens/FixedBuffer.h"
#include "aliens/tcp/TCPAcceptServer.h"
#include "aliens/tcp/TCPServerSession.h"
#include "aliens/memory.h"

using namespace std;
using asio_tcp = boost::asio::ip::tcp;
using aliens::Buffer;
using aliens::tcp::TCPAcceptServer;
using aliens::tcp::TCPServerSession;


class SessionHandlerFactory {
 public:
  virtual TCPServerSession::EventHandler* getHandler() = 0;
};


class EchoSessionHandler: public TCPServerSession::EventHandler {
 protected:
  std::atomic<bool> running_ {true};
  std::atomic<size_t> counter_ {0};
  void setRunning(bool isRunning) {
    running_.store(isRunning);
  }
 public:
  void onWriteSuccess(size_t nr) override {
    LOG(INFO) << "onWriteSuccess! " << nr;
    if (running_.load()) {
      read(aliens::makeUnique<Buffer>());
    }
  }
  void onReadSuccess(std::unique_ptr<Buffer> buff) override {
    LOG(INFO) << "onReadSuccess!";
    auto msg = buff->copyToString();
    LOG(INFO) << "onReadSuccess!\t\t" << msg;
    if (counter_.fetch_add(1) >= 4) {
      close();
    } else {
      write(std::move(buff));
    }
  }

  void onWriteError(boost::system::error_code ec, size_t nr) override {
    LOG(INFO) << "onWriteError! " << ec;
  }
  void onReadError(boost::system::error_code ec) override {
    LOG(INFO) << "onReadError! " << ec;
  }
  void onConnected() override {
    read(aliens::makeUnique<Buffer>());
  }
  void onBeforeClose() override {
    LOG(INFO) << "onBeforeClose";
  }
  void onAfterClose() override {
    LOG(INFO) << "onAfterClose";
  }
};


class EchoSessionHandlerFactory : public SessionHandlerFactory {
 public:
  TCPServerSession::EventHandler* getHandler() override {
    return new EchoSessionHandler;
  }
};


class HandlerFactoryAcceptHandler : public TCPAcceptServer::EventHandler {
 protected:
  std::unique_ptr<SessionHandlerFactory> sessionHandlerFactory_ {nullptr};
 public:
  HandlerFactoryAcceptHandler(std::unique_ptr<SessionHandlerFactory> &&factory)
    : sessionHandlerFactory_(std::move(factory)) {}
  void onAcceptSuccess(asio_tcp::socket&& sock) override {
    // obviously this needs memory management.
    auto session = new TCPServerSession(
      std::move(sock), sessionHandlerFactory_->getHandler()
    );
    (void) session; // unused
  }
  void onAcceptError(boost::system::error_code ec) override {
    LOG(INFO) << "onAcceptError: " << ec;
  }
  void onStarted() override {
    LOG(INFO) << "onStarted!";
    startAccepting();
  }
};


int main() {
  google::InstallFailureSignalHandler();
  LOG(INFO) << "start";
  short portNo = 5097;
  thread serverThread([portNo]() {
    try {
      boost::asio::io_service ioService;
      auto handlerFactory = aliens::makeUnique<EchoSessionHandlerFactory>();
      auto handler = new HandlerFactoryAcceptHandler(
        aliens::makeUnique<EchoSessionHandlerFactory>()
      );
      auto server = std::make_shared<TCPAcceptServer>(
        ioService, handler, asio_tcp::endpoint(asio_tcp::v4(), portNo)
      );
      server->start();
      ioService.run();
    } catch (std::exception &ex) {
      LOG(INFO) << "err! " << ex.what();
    }
  });
  serverThread.join();
  LOG(INFO) << "end";
}