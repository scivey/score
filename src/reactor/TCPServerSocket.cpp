#include "aliens/reactor/TCPServerSocket.h"
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/exceptions/macros.h"
#include "aliens/ScopeGuard.h"
#include "aliens/reactor/SocketAddr.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <glog/logging.h>

namespace aliens { namespace reactor {

TCPServerSocket::TCPServerSocket(FileDescriptor &&desc,
    EventHandler *handler,
    const SocketAddr& localAddr,
    const SocketAddr &remoteAddr)
  : FdHandlerBase<TCPServerSocket>(std::forward<FileDescriptor>(desc)),
    handler_(handler),
    localAddr_(localAddr),
    remoteAddr_(remoteAddr) {}

void TCPServerSocket::triggerReadable() {
  LOG(INFO) << "onReadable";
  const size_t kReadBuffSize = 128;
  for (;;) {
    void *buffPtr {nullptr};
    size_t buffLen {0};
    handler_->getReadBuffer(&buffPtr, &buffLen, kReadBuffSize);
    CHECK(!!buffPtr);
    ssize_t nr = read(getFdNo(), (char*) buffPtr, buffLen);
    if (nr == -1) {
      if (errno == EAGAIN) {
        handler_->readBufferAvailable(buffPtr, 0);
        break;
      } else {
        ALIENS_CHECK_SYSCALL2(nr, "read()");
      }
    } else if (nr == 0) {
      handler_->readBufferAvailable(buffPtr, 0);
      handler_->onEOF();
      break;
    } else {
      handler_->readBufferAvailable(buffPtr, nr);
    }
  }
}

void TCPServerSocket::triggerWritable() {
  handler_->onWritable();
}

void TCPServerSocket::triggerError() {
  LOG(INFO) << "onError [" << errno << "]";
}

TCPServerSocket TCPServerSocket::fromAccepted(
    FileDescriptor &&fd,
    TCPServerSocket::EventHandler *handler,
    const SocketAddr &localAddr,
    const SocketAddr &remoteAddr) {
  return TCPServerSocket(
    std::forward<FileDescriptor>(fd),
    handler,
    localAddr,
    remoteAddr
  );
}

}} // aliens::reactor