#include "score/net/TCPChannel.h"
#include "score/reactor/FdHandlerBase.h"
#include "score/exceptions/macros.h"
#include "score/ScopeGuard.h"
#include "score/macros.h"
#include "score/net/SocketAddr.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <glog/logging.h>

using score::io::NonOwnedBufferPtr;
using score::async::ErrBack;
using score::exceptions::BaseError;
using score::posix::FileDescriptor;
namespace score { namespace net {

TCPChannel::TCPChannel(FileDescriptor &&desc,
    EventHandler *handler,
    const TCPConnectionInfo& connInfo)
  : FdHandlerBase<TCPChannel>(std::forward<FileDescriptor>(desc)),
    handler_(handler),
    connInfo_(connInfo) {
  handler_->setParent(this);
}


void TCPChannel::EventHandler::readSome() {
  getParent()->readSome();
}

void TCPChannel::readSome() {
  const size_t kReadBuffSize = 128;
  for (;;) {
    void *buffPtr {nullptr};
    size_t buffLen {0};
    handler_->getReadBuffer(&buffPtr, &buffLen, kReadBuffSize);
    SDCHECK(!!buffPtr);
    ssize_t nr = read(getFdNo(), (char*) buffPtr, buffLen);
    if (nr == -1) {
      if (errno == EAGAIN) {
        handler_->onReadableStop();
        break;
      } else {
        SCORE_CHECK_SYSCALL2(nr, "read()");
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

void TCPChannel::triggerReadable() {
  handler_->onReadableStart();
}

void TCPChannel::triggerWritable() {
  handler_->onWritable();
}

void TCPChannel::triggerError() {
  VLOG(50) << "onError [" << errno << "]";
}

void TCPChannel::EventHandler::sendBuff(
    NonOwnedBufferPtr buff, ErrBack &&cb) {
  getParent()->sendBuff(buff, std::forward<ErrBack>(cb));
}

void TCPChannel::sendBuff(
    NonOwnedBufferPtr buff, ErrBack &&cb) {
  ssize_t nr = ::write(getFdNo(), buff.vdata(), buff.size());
  SCORE_CHECK_SYSCALL2(nr, "write()");
  if (nr == buff.size()) {
    cb();
  } else {
    cb(BaseError("couldn't write everything."));
  }
}

void TCPChannel::EventHandler::shutdown() {
  getParent()->shutdown();
}

void TCPChannel::shutdown() {
  VLOG(50) << "TCPChannel::shutdown()";
  getFileDescriptor().close();
}

TCPChannel TCPChannel::create(FileDescriptor &&fd,
    EventHandler *handler, const TCPConnectionInfo &info) {
  return TCPChannel(
    std::forward<FileDescriptor>(fd),
    handler, info
  );
}

TCPChannel* TCPChannel::createPtr(FileDescriptor &&fd,
    EventHandler *handler, const TCPConnectionInfo &info) {
  return new TCPChannel(
    std::forward<FileDescriptor>(fd),
    handler, info
  );
}


}} // score::reactor