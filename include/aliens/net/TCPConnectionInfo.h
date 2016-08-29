#pragma once
#include "aliens/net/SocketAddr.h"

namespace aliens { namespace net {

struct TCPConnectionInfo {
  SocketAddr localAddr;
  SocketAddr remoteAddr;
};

}} // aliens::net

