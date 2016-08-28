#pragma once
#include <glog/logging.h>
#include <glog/logging.h>
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/reactor/SocketAddr.h"
#include "aliens/reactor/ParentHaving.h"
#include "aliens/async/ErrBack.h"
#include "aliens/io/NonOwnedBufferPtr.h"

namespace aliens { namespace reactor {

class TCPClientSocket : public FdHandlerBase<TCPClientSocket> {
 public:
  class EventHandler: public ParentHaving<TCPClientSocket> {
   public:
    virtual void getReadBuffer(void** buff, size_t* buffLen, size_t hint) = 0;
    virtual void readBufferAvailable(void *buff, size_t buffLen) = 0;
    virtual void onConnectSuccess() = 0;
    virtual void onConnectError(int err) = 0;
    virtual void onReadableStart() = 0;
    virtual void onReadableStop() = 0;
    virtual void onWritable() = 0;
    virtual void onEOF() = 0;
    void readSome();
    void sendBuff(io::NonOwnedBufferPtr, async::ErrBack &&errback);
    void shutdown();
    virtual ~EventHandler() = default;
  };
  friend class EventHandler;
 protected:
  EventHandler *handler_ {nullptr};
  SocketAddr remoteAddr_;
  TCPClientSocket(FileDescriptor &&, EventHandler*, const SocketAddr&);
  void readSome();
 public:
  void triggerReadable();
  void triggerWritable();
  void triggerError();
  void sendBuff(io::NonOwnedBufferPtr, async::ErrBack &&errback);
  void shutdown();
  static TCPClientSocket connect(
    SocketAddr remoteAddr, EventHandler*
  );
  static std::shared_ptr<TCPClientSocket> connectShared(
    SocketAddr remoteAddr, EventHandler*
  );

};

}} // aliens::reactor