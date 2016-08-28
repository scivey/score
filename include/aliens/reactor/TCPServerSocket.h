#pragma once
#include <glog/logging.h>
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/reactor/SocketAddr.h"
namespace aliens { namespace reactor {

class TCPServerSocket : public FdHandlerBase<TCPServerSocket> {
 public:
  class EventHandler {
   protected:
    TCPServerSocket *parent_ {nullptr};
    TCPServerSocket* getParent() {
      return parent_;
    }
    void setParent(TCPServerSocket *sock) {
      parent_ = sock;
    }
   public:
    virtual void getReadBuffer(void **buff, size_t* buffLen, size_t hint) = 0;
    virtual void readBufferAvailable(void *buff, size_t buffLen) = 0;
    virtual void onWritable() = 0;
    virtual void onEOF() = 0;
    virtual ~EventHandler() = default;
  };
 protected:
  EventHandler *handler_ {nullptr};
  SocketAddr localAddr_;
  SocketAddr remoteAddr_;
  TCPServerSocket(FileDescriptor &&, EventHandler*,
    const SocketAddr &localAddr, const SocketAddr &remoteAddr
  );
 public:
  void triggerReadable();
  void triggerWritable();
  void triggerError();
  static TCPServerSocket fromAccepted(
    FileDescriptor&&, EventHandler*,
    const SocketAddr&, const SocketAddr&
  );
};

}} // aliens::reactor