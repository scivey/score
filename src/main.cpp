#include <thread>
#include <string>
#include <sstream>
#include <memory>

#include <glog/logging.h>
#include <boost/asio.hpp>
using namespace std;
using boost::asio::ip::tcp;


std::string buffToString(char *buff, size_t buffLen) {
  std::ostringstream result;
  char *current = buff;
  for (size_t i = 0; i < buffLen; i++) {
    char c = *current;
    if (c == '\0') {
      break;
    }
    if (c == '\n') {
      if (i < (buffLen - 1)) {
        result << c;
      } else {
        break;
      }
    } else {
      result << c;
    }
    current++;
  }
  return result.str();
}

template<size_t NBytes>
class FixedBuffer {
 public:
  static const size_t N = NBytes;
 protected:
  char data_[NBytes];
 public:
  FixedBuffer() {
    zero();
  }
  char* body() {
    return (char*) data_;
  }
  void zero() {
    memset((char*) data_, '\0', NBytes);
  }
  size_t currentLen() const {
    return strlen(data_);
  }
  size_t remaining() const {
    return NBytes - currentLen();
  }
  size_t capacity() const {
    return NBytes;
  }
  std::string toString() {
    return buffToString(body(), currentLen());
  }
};

using Buffer = FixedBuffer<1024>;

class TCPRequestHandler : public std::enable_shared_from_this<TCPRequestHandler> {
 protected:
  tcp::socket socket_;
  Buffer buff_;
 public:
  TCPRequestHandler(tcp::socket socket): socket_(std::move(socket)) {}
  void start() {
    startReading();
  }
  ~TCPRequestHandler() {
    LOG(INFO) << "~TCPRequestHandler()";
  }
 protected:
  void readSome() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buff_.body(), buff_.capacity()),
      [this, self](boost::system::error_code ec, std::size_t nr) {
        if (ec) {
          LOG(INFO) << "err ! " << ec;
        } else {
          LOG(INFO) << "nr : " << nr;
          auto result = buff_.toString();
          LOG(INFO) << "received : '" << result << "'";
        }
      }
    );
  }
  void startReading() {
    LOG(INFO) << "startReading!!";
    readSome();
  }
  void write(Buffer *buff) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
      boost::asio::buffer(buff->body(), buff->currentLen()),
      [this, self](boost::system::error_code ec, std::size_t nr) {
        LOG(INFO) << "nr : " << nr;
      }
    );
  }
};

class TCPServer: public std::enable_shared_from_this<TCPServer> {
 protected:
  tcp::acceptor acceptor_;
  tcp::socket socket_;
 public:
  TCPServer(boost::asio::io_service &ioService, const tcp::endpoint &tcpEndpoint)
    : acceptor_(ioService, tcpEndpoint), socket_(ioService) {}
  void listen() {
    doAccept();
  }
 protected:
  void doAccept() {
    auto self = shared_from_this();
    acceptor_.async_accept(socket_,
      [self, this](boost::system::error_code ec) {
        if (ec) {
          LOG(INFO) << "err ! " << ec;
        } else {
          std::make_shared<TCPRequestHandler>(std::move(socket_))->start();
        }
        doAccept();
      }
    );
  }
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