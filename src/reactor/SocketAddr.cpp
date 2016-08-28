#include "aliens/reactor/SocketAddr.h"
#include "aliens/exceptions/exceptions.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

namespace aliens { namespace reactor {

SocketAddr::SocketAddr(const std::string &host, short port)
  : host_(host), port_(port){}

SocketAddr::SocketAddr(const std::string &host, const std::string &port)
  : host_(host), port_( (short) std::strtoul(port.c_str(), nullptr, 0) ) {}

const std::string& SocketAddr::getHost() const {
  return host_;
}

short SocketAddr::getPort() const {
  return port_;
}

struct sockaddr_in SocketAddr::to_sockaddr_in() const {
  struct sockaddr_in result;
  memset(&result, 0, sizeof(result));
  result.sin_family = AF_INET;
  result.sin_port = htons(port_);
  if (inet_aton(host_.c_str(), &result.sin_addr) == 0) {
    throw exceptions::BaseError("invalid address.");
  }
  return result;
}

}} // aliens::reactor
