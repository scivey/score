#include "aliens/net/SocketAddr.h"
#include "aliens/exceptions/exceptions.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

using aliens::io::Scstring;

namespace aliens { namespace net {

SocketAddr::SocketAddr(){}

SocketAddr::SocketAddr(Scstring &&host, short port)
  : host_(std::forward<Scstring>(host)), port_(port){}

SocketAddr::SocketAddr(Scstring &&host, Scstring &&port)
  : host_(std::forward<Scstring>(host)),
    port_( (short) std::strtoul(port.c_str(), nullptr, 0) ) {}

const Scstring& SocketAddr::getHost() const {
  return host_;
}

void SocketAddr::setHost(Scstring &&host) {
  host_ = std::forward<Scstring>(host);
}

void SocketAddr::setHost(const Scstring &host) {
  host_ = host;
}


void SocketAddr::setPort(short portNo) {
  port_ = portNo;
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

}} // aliens::net
