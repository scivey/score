#include "aliens/reactor/TCPClient.h"
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
using aliens::exceptions::SystemError;


namespace aliens { namespace reactor {

using channel_ptr = typename TCPClient::channel_ptr;

TCPClient::TCPClient(EpollReactor *reactor, channel_ptr channel)
  : reactor_(reactor), channel_(std::forward<channel_ptr>(channel)) {}


TCPClient TCPClient::connect(
    EpollReactor *reactor, EventHandler *handler,
    const SocketAddr &addr) {
  const int protocolPlaceholder = 0;
  int sockFd = ::socket(
    AF_INET,
    SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
    protocolPlaceholder
  );
  ALIENS_CHECK_SYSCALL2(sockFd, "socket()");
  auto addrIn = addr.to_sockaddr_in();
  socklen_t addrLen = sizeof addrIn;
  int rc = ::connect(sockFd, (struct sockaddr*) &addrIn, addrLen);
  if (rc < 0 && errno != EINPROGRESS) {
    throw SystemError::fromErrno(errno, "connect()");
  }
  TCPConnectionInfo connInfo;
  connInfo.remoteAddr.setHost(addr.getHost());
  connInfo.remoteAddr.setPort(addr.getPort());
  return TCPClient(
    reactor,
    TCPChannel::fromDescriptorUnique(
      FileDescriptor::fromIntExcept(sockFd),
      handler, connInfo
    )
  );
}

TCPClient* TCPClient::connectPtr(EpollReactor *reactor,
    EventHandler *handler, const SocketAddr &addr) {
  return new TCPClient(
    TCPClient::connect(reactor, handler, addr)
  );
}

TCPChannel::EpollTask* TCPClient::getEpollTask() {
  return channel_->getEpollTask();
}

}} // aliens::reactor