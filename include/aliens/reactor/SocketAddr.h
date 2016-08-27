#pragma once
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace aliens { namespace reactor {

class SocketAddr {
 protected:
  std::string host_;
  short port_;
 public:
  SocketAddr(const std::string &host, short port);
  const std::string& getHost() const;
  short getPort() const;
  struct sockaddr_in to_sockaddr_in() const;
};

}} // aliens::reactor
