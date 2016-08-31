#pragma once
#include "score/net/SocketAddr.h"

namespace score { namespace net {

struct TCPConnectionInfo {
  SocketAddr localAddr;
  SocketAddr remoteAddr;
};

}} // score::net

