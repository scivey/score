#include "aliens/reactor/TCPSocket.h"
#include "aliens/exceptions/exceptions.h"
#include "aliens/exceptions/macros.h"
#include "aliens/ScopeGuard.h"

#include <glog/logging.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

using aliens::exceptions::SystemError;
using aliens::exceptions::BaseError;

namespace aliens { namespace reactor {

TCPSocket::TCPSocket(FileDescriptor &&fd)
  : fd_(std::forward<FileDescriptor>(fd)){}

int TCPSocket::getFdNo() const {
  return fd_.getFdNo();
}

TCPSocket TCPSocket::fromAccepted(FileDescriptor &&fd, const char* remoteHost, const char *remotePort) {
  TCPSocket sock(std::forward<FileDescriptor>(fd));
  sock.remoteHost_ = remoteHost;
  int remPort = atol(remotePort);
  CHECK(remPort <= 65535);
  sock.remotePort_ = (short) remPort;
  return sock;
}

TCPSocket TCPSocket::connect(SocketAddr addr) {
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
  auto desc = FileDescriptor::fromIntExcept(sockFd);
  TCPSocket sock(std::move(desc));
  sock.remotePort_ = addr.getPort();
  sock.remoteHost_ = addr.getHost();
  return sock;
}

TCPSocket TCPSocket::bindPort(short portno) {
  std::ostringstream oss;
  oss << portno;
  auto portStr = oss.str();
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC; // either ipv4 or ipv6
  hints.ai_socktype = SOCK_STREAM; // tcp
  hints.ai_flags = AI_PASSIVE; // any interface
  struct addrinfo *result, *rp;
  aliens::ScopeGuard guard([&result]() {
    if (result) {
      freeaddrinfo(result);
    }
  });
  ALIENS_CHECK_SYSCALL(getaddrinfo(nullptr, portStr.c_str(), &hints, &result));
  int status, sfd;
  for (rp = result; rp != nullptr; rp = rp->ai_next) {
    sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sfd == -1) {
      continue;
    }
    {
      int shouldReuse = 1;
      ALIENS_CHECK_SYSCALL(setsockopt(
        sfd, SOL_SOCKET, SO_REUSEADDR,
        (const char*) &shouldReuse,
        sizeof(shouldReuse)
      ));
      ALIENS_CHECK_SYSCALL(setsockopt(
        sfd, SOL_SOCKET, SO_REUSEPORT,
        (const char*) &shouldReuse,
        sizeof(shouldReuse)
      ));
    }
    status = bind(sfd, rp->ai_addr, rp->ai_addrlen);
    if (status == 0) {
      break;
    }
    close(sfd);
  }
  if (!rp) {
    throw BaseError("could not bind.");
  }
  auto desc = FileDescriptor::fromIntExcept(sfd);
  desc.makeNonBlocking();
  TCPSocket sock(std::move(desc));
  sock.localPort_ = portno;
  return sock;
}

bool TCPSocket::valid() const {
  return fd_.valid();
}

void TCPSocket::listen() {
  ALIENS_CHECK_SYSCALL(::listen(fd_.getFdNo(), SOMAXCONN));
}

void TCPSocket::stop() {
  fd_.close();
}

}} // aliens::reactor
