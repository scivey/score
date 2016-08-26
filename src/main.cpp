#include <thread>
#include <string>
#include <sstream>
#include <atomic>

#include <memory>
#include "aliens/MoveWrapper.h"
#include <glog/logging.h>
#include <boost/asio.hpp>
#include "aliens/FixedBuffer.h"
#include "aliens/tcp/TCPAcceptServer.h"
#include "aliens/tcp/TCPSocket.h"
#include "aliens/tcp/TCPServer.h"
#include "aliens/tcp/TCPClient.h"
#include "aliens/async/IOService.h"
#include "aliens/mem/util.h"
#include "aliens/tcp/EchoServer.h"

using namespace std;
using asio_tcp = boost::asio::ip::tcp;
using aliens::Buffer;
using aliens::io::SocketAddr;

using aliens::tcp::TCPAcceptServer;
using aliens::tcp::TCPSocket;
using aliens::tcp::TCPClient;

using aliens::async::IOService;
using aliens::tcp::TCPServer;
using aliens::tcp::EchoServer;


class EchoClientHandler: public TCPSocket::EventHandler {
 protected:
  std::vector<std::string> messages_;
 public:
  EchoClientHandler(const std::vector<std::string> messages)
    : messages_(messages) {}


  void maybeWrite() {
    if (messages_.empty()) {
      LOG(INFO) << "out of messages!!";
    } else {
      auto msg = messages_.back();
      messages_.pop_back();
      LOG(INFO) << "going to send: '" << msg << "'";
      auto toSend = aliens::mem::makeUnique<Buffer>();
      toSend->fillWith(msg);
      write(std::move(toSend));
    }
  }
  void onConnectSuccess() override {
    LOG(INFO) << "onConnectSuccess..";
    maybeWrite();
  }
  void onConnectError(boost::system::error_code ec) override {
    LOG(INFO) << "onConnectError '" << ec << "'";
  }
  void onWriteSuccess(size_t nr) override {
    LOG(INFO) << "onWriteSuccess : " << nr;
    read(aliens::mem::makeUnique<Buffer>());
  }
  void onWriteError(boost::system::error_code ec, size_t nr) override {
    LOG(INFO) << "onWriteError : " << ec << " [" << nr << "]";
  }
  void onReadSuccess(std::unique_ptr<Buffer> buff) override {
    LOG(INFO) << "onReadSuccess!!! '" << buff->copyToString() << "'";
    if (messages_.empty()) {
      LOG(INFO) << "out of messages!!";
    } else {
      auto msg = messages_.back();
      messages_.pop_back();
      LOG(INFO) << "going to send: '" << msg << "'";
      auto toSend = aliens::mem::makeUnique<Buffer>();
      toSend->fillWith(msg);
      write(std::move(toSend));
    }
  }
  void onReadError(boost::system::error_code ec) override {
    LOG(INFO) << "onReadError...";
  }
  void onAfterClose() override {
    LOG(INFO) << "onAfterClose.";
  }
};

void runEcho() {
  LOG(INFO) << "echo start";
  short portNo = 5017;
  auto ioService = new IOService;
  thread serverThread([portNo, ioService]() {
    try {
      auto server = std::make_shared<EchoServer>(ioService);
      server->listen(portNo);
      ioService->run();
    } catch (std::exception &ex) {
      LOG(INFO) << "err! " << ex.what();
    } catch (...) {
      auto ex2 = std::current_exception();
      LOG(INFO) << "BAD!";
    }
  });
  this_thread::sleep_for(chrono::milliseconds(50));
  thread clientThread([portNo, ioService]() {
      SocketAddr addr;
      addr.host = "localhost";
      addr.port = portNo;
      std::vector<std::string> messages {
        "one", "two", "three", "four", "five"
      };
      auto client = std::make_shared<TCPClient>(ioService, new EchoClientHandler(messages), addr);
      client->start();
  });
  serverThread.join();
  clientThread.join();
  LOG(INFO) << "end";
}

int main() {
  google::InstallFailureSignalHandler();
  runEcho();
}