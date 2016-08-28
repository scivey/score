#include "aliens/reactor/TCPAcceptSocket.h"
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/exceptions/macros.h"
#include "aliens/ScopeGuard.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <glog/logging.h>

namespace aliens { namespace reactor {

TCPAcceptSocket::TCPAcceptSocket(FileDescriptor &&desc,
    EventHandler *handler)
  : FdHandlerBase<TCPAcceptSocket>(std::forward<FileDescriptor>(desc)),
    handler_(handler) {
  handler_->setParent(this);
}

void TCPAcceptSocket::onReadable() {
  LOG(INFO) << "onReadable";
  for (;;) {
    struct sockaddr inAddr;
    socklen_t inLen;
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    inLen = sizeof inAddr;
    int inFd = accept(getFdNo(), &inAddr, &inLen);
    if (inFd == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // all incoming connections processed.
        break;
      } else {
        handler_->onAcceptError(errno);
        break;
      }
    }
    int status = getnameinfo(
      &inAddr, inLen,
      hbuf, sizeof hbuf,
      sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV);
    if (status == 0) {
      handler_->onAcceptSuccess(inFd, hbuf, sbuf);
    } else {
      handler_->onAcceptError(errno);
    }
  }
}

void TCPAcceptSocket::onWritable() {
  LOG(INFO) << "onWritable";
}

void TCPAcceptSocket::onError() {
  LOG(INFO) << "onError [" << errno << "]";
}

TCPAcceptSocket TCPAcceptSocket::bindPort(short portNo,
    TCPAcceptSocket::EventHandler *handler) {
  std::ostringstream oss;
  oss << portNo;
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
    } else {
      close(sfd);
    }
  }
  if (!rp) {
    throw exceptions::BaseError("could not bind.");
  }
  // auto desc = FileDescriptor::fromIntExcept(sfd);
  // desc.makeNonBlocking();
  // TCPAcceptSocket sock(std::move(desc), handler);
  TCPAcceptSocket sock(
    FileDescriptor::fromIntExcept(sfd),
    handler
  );
  sock.getFileDescriptor().makeNonBlocking();
  return sock;
}

void TCPAcceptSocket::listen() {
  LOG(INFO) << "listen()";
  ALIENS_CHECK_SYSCALL(::listen(getFdNo(), SOMAXCONN));
}

}} // aliens::reactor