#pragma once
#include <boost/asio.hpp>
#include <memory>

namespace score { namespace async {

class IOService: public std::enable_shared_from_this<IOService> {
 protected:
  boost::asio::io_service boostService_;
  IOService(const IOService&) = delete;
  IOService& operator=(const IOService&) = delete;

 public:

  IOService(IOService&&) = default;
  IOService& operator=(IOService&&) = default;
  IOService(){}
  boost::asio::io_service& getBoostService() {
    return boostService_;
  }
  void run() {
    boostService_.run();
  }
};

}} // score::async