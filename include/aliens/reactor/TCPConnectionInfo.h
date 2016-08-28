#pragma once
#include "aliens/reactor/SocketAddr.h"

namespace aliens { namespace reactor {

struct TCPConnectionInfo {
  SocketAddr localAddr;
  SocketAddr remoteAddr;
};

}} // aliens::reactor
