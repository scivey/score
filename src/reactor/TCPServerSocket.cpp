#include "aliens/reactor/TCPServerSocket.h"
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/exceptions/macros.h"
#include "aliens/ScopeGuard.h"
#include "aliens/macros.h"

#include "aliens/reactor/SocketAddr.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <glog/logging.h>

using aliens::io::NonOwnedBufferPtr;
using aliens::async::ErrBack;
using aliens::exceptions::BaseError;
using std::unique_ptr;

namespace aliens { namespace reactor {

using ChannelPtr = unique_ptr<TCPServerSocket>;

TCPServerSocket::TCPServerSocket(ChannelPtr channel)
  : channel_(std::forward<ChannelPtr>(channel)) {}

TCPServerSocket TCPServerSocket::fromAccepted(ChannelPtr channel) {
  return TCPServerSocket(std::forward<ChannelPtr>(channel));
}
TCPServerSocket* TCPServerSocket::fromAcceptedPtr(ChannelPtr channel) {
  return new TCPServerSocket(std::forward<ChannelPtr>(channel));
}
std::shared_ptr<TCPServerSocket> TCPServerSocket::fromAcceptedShared(
    ChannelPtr channel) {
  return std::shared_ptr<TCPServerSocket>(
    TCPServerSocket::fromAcceptedPtr(std::forward<ChannelPtr>(channel))
  );
}


}} // aliens::reactor