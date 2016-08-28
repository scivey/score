#pragma once
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace aliens { namespace reactor {

class SocketAddr {
 protected:
  std::string host_ {""};
  short port_ {0};
 public:
  SocketAddr();
  SocketAddr(const std::string &host, short port);
  SocketAddr(const std::string &host, const std::string &port);
  const std::string& getHost() const;
  short getPort() const;
  void setPort(short);
  void setHost(const std::string&);
  struct sockaddr_in to_sockaddr_in() const;
};

}} // aliens::reactor
