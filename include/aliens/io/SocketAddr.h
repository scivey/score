#pragma once
#include <string>
namespace aliens { namespace io {

struct SocketAddr {
  std::string host;
  short port;
};

}} // aliens::io