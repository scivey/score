#pragma once

#include <memory>
#include <glog/logging.h>
#include <boost/asio.hpp>
#include "aliens/FixedBuffer.h"
#include "aliens/tcp/TCPAcceptServer.h"
#include "aliens/tcp/TCPSocket.h"
#include "aliens/tcp/TCPServer.h"
#include "aliens/async/IOService.h"

namespace aliens { namespace tcp {

class EchoSocketHandler: public TCPSocket::EventHandler {
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

  void onWriteError(boost::system::error_code ec, size_t) override {
    LOG(INFO) << "onWriteError! " << ec;
  }
  void onReadError(boost::system::error_code ec) override {
    LOG(INFO) << "onReadError! " << ec;
  }
  void onConnectSuccess() override {
    read(aliens::makeUnique<Buffer>());
  }
  void onConnectError(boost::system::error_code ec) override {
    LOG(INFO) << "onConnectError!!";
  }
  void onBeforeClose() override {
    LOG(INFO) << "onBeforeClose";
  }
  void onAfterClose() override {
    LOG(INFO) << "onAfterClose";
  }
};


class EchoSocketHandlerFactory : public TCPSocket::EventHandlerFactory {
 public:
  TCPSocket::EventHandler* getHandler() override {
    return new EchoSocketHandler;
  }
};


class EchoServer: public std::enable_shared_from_this<EchoServer> {
 protected:
  async::IOService *ioService_ {nullptr};
  std::shared_ptr<TCPServer> tcpServer_ {nullptr};
 public:
  EchoServer(async::IOService *ioService): ioService_(ioService){}
  void listen(short portNo) {
    CHECK(!tcpServer_);
    tcpServer_ = std::make_shared<TCPServer>(
      ioService_, std::make_shared<EchoSocketHandlerFactory>()
    );
    tcpServer_->listen(portNo);
  }
};

}} // aliens::tcp
