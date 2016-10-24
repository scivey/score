#pragma once
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "score/io/Scstring.h"

namespace score { namespace io {

class SocketAddr {
 protected:
  io::Scstring host_ {""};
  short port_ {0};
 public:
  SocketAddr();
  SocketAddr(io::Scstring &&host, short port);
  SocketAddr(io::Scstring &&host, io::Scstring &&port);
  const io::Scstring& getHost() const;
  short getPort() const;
  void setPort(short);
  void setHost(io::Scstring &&host);
  void setHost(const io::Scstring &host);
  struct sockaddr_in to_sockaddr_in() const;
};

}} // score::io
