#pragma once
#include <glog/logging.h>
#include <glog/logging.h>
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/reactor/SocketAddr.h"
#include "aliens/reactor/ParentHaving.h"
#include "aliens/async/ErrBack.h"
#include "aliens/io/NonOwnedBufferPtr.h"

namespace aliens { namespace reactor {

class TCPChannel : public FdHandlerBase<TCPChannel> {
 public:
  class EventHandler: public ParentHaving<TCPChannel> {
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

  struct ConnectionInfo {
    SocketAddr localAddr;
    SocketAddr remoteAddr;
  };

 protected:
  EventHandler *handler_ {nullptr};
  ConnectionInfo connInfo_;
  TCPChannel(FileDescriptor &&, EventHandler*, const ConnectionInfo&);
  void readSome();

  static TCPChannel* fromDescriptorPtr(
    FileDescriptor&&fd, EventHandler *handler,
    const ConnectionInfo &info
  );
 public:
  void triggerReadable();
  void triggerWritable();
  void triggerError();
  void sendBuff(io::NonOwnedBufferPtr, async::ErrBack &&errback);
  void shutdown();
  static TCPChannel fromDescriptor(
    FileDescriptor&&fd, EventHandler *handler,
    const ConnectionInfo &info
  );
  static TCPChannel fromDescriptorShared(
    FileDescriptor&&fd, EventHandler *handler,
    const ConnectionInfo &info
  );
};

}} // aliens::reactor