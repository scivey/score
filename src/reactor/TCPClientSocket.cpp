#include "aliens/reactor/TCPClientSocket.h"
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

TCPClientSocket::TCPClientSocket(FileDescriptor &&desc,
    EventHandler *handler,
    const SocketAddr &remoteAddr)
  : FdHandlerBase<TCPClientSocket>(std::forward<FileDescriptor>(desc)),
    handler_(handler),
    remoteAddr_(remoteAddr) {
  handler_->setParent(this);
}

void TCPClientSocket::readSome() {
  LOG(INFO) << "readSome";
  const size_t kReadBuffSize = 128;
  for (;;) {
    void *buffPtr {nullptr};
    size_t buffLen {0};
    handler_->getReadBuffer(&buffPtr, &buffLen, kReadBuffSize);
    CHECK(!!buffPtr);
    ssize_t nr = read(getFdNo(), (char*) buffPtr, buffLen);
    if (nr == -1) {
      if (errno == EAGAIN) {
        handler_->onReadableStop();
        break;
      } else {
        ALIENS_CHECK_SYSCALL2(nr, "read()");
      }
    } else if (nr == 0) {
      handler_->onReadableStop();
      handler_->onEOF();
      break;
    } else {
      handler_->readBufferAvailable(buffPtr, nr);
    }
  }
}

void TCPClientSocket::triggerWritable() {
  handler_->onWritable();
}


void TCPClientSocket::triggerReadable() {
  handler_->onReadableStart();
}

void TCPClientSocket::triggerError() {
  LOG(INFO) << "onError [" << errno << "]";
}

void TCPClientSocket::EventHandler::sendBuff(
    NonOwnedBufferPtr buff, ErrBack &&cb) {
  getParent()->sendBuff(buff, std::forward<ErrBack>(cb));
}

void TCPClientSocket::sendBuff(
    NonOwnedBufferPtr buff, ErrBack &&cb) {
  auto buffStr = buff.copyToString();
  LOG(INFO) << "should send : [" << buffStr.size()
      << "] '" << buffStr << "'";
  auto fd = getFdNo();
  // ALIENS_UNUSED(fd);
  cb();
  ssize_t nr = ::write(fd, buff.vdata(), buff.size());
  ALIENS_CHECK_SYSCALL2(nr, "write()");
  if (nr == buff.size()) {
    cb();
  } else {
    cb(BaseError("couldn't write everything."));
  }
}

void TCPClientSocket::EventHandler::shutdown() {
  getParent()->shutdown();
}

void TCPClientSocket::shutdown() {
  LOG(INFO) << "TCPClientSocket::shutdown()";
  getFileDescriptor().close();
}


TCPClientSocket TCPClientSocket::connect(
    SocketAddr addr, TCPClientSocket::EventHandler* handler) {
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
  return TCPClientSocket(
    FileDescriptor::fromIntExcept(sockFd),
    handler, addr
  );
}

std::shared_ptr<TCPClientSocket> TCPClientSocket::connectShared(
    SocketAddr addr, TCPClientSocket::EventHandler* handler) {
  return std::shared_ptr<TCPClientSocket>(new TCPClientSocket(
    TCPClientSocket::connect(addr, handler)
  ));
}

}} // aliens::reactor