#include "score/net/TCPClient.h"
#include "score/reactor/FdHandlerBase.h"
#include "score/exceptions/macros.h"
#include "score/ScopeGuard.h"
#include "score/macros.h"
#include "score/PointerFactory.h"
#include "score/net/SocketAddr.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <glog/logging.h>

using score::net::SocketAddr;
using score::io::NonOwnedBufferPtr;
using score::async::ErrBack;
using score::exceptions::BaseError;
using score::exceptions::SystemError;
using score::reactor::EpollReactor;
using score::posix::FileDescriptor;



namespace score { namespace net {

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
  SCORE_CHECK_SYSCALL2(sockFd, "socket()");
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
    PointerFactory<TCPChannel>::createUnique(
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

}} // score::reactor