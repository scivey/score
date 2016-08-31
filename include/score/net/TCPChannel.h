#pragma once
#include <glog/logging.h>
#include <glog/logging.h>
#include "score/reactor/FdHandlerBase.h"
#include "score/net/SocketAddr.h"
#include "score/ParentHaving.h"
#include "score/PointerFactory.h"

#include "score/net/TCPConnectionInfo.h"
#include "score/async/ErrBack.h"
#include "score/io/NonOwnedBufferPtr.h"

namespace score { namespace net {

class TCPChannel : public reactor::FdHandlerBase<TCPChannel> {
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

 protected:
  EventHandler *handler_ {nullptr};
  TCPConnectionInfo connInfo_;
  TCPChannel(posix::FileDescriptor &&, EventHandler*, const TCPConnectionInfo&);
  void readSome();

  static TCPChannel* createPtr(
    posix::FileDescriptor&&fd, EventHandler *handler,
    const TCPConnectionInfo &info
  );
  friend class PointerFactory<TCPChannel>;
 public:
  using Factory = PointerFactory<TCPChannel>;
  void triggerReadable();
  void triggerWritable();
  void triggerError();
  void sendBuff(io::NonOwnedBufferPtr, async::ErrBack &&errback);
  void shutdown();
  static TCPChannel create(
    posix::FileDescriptor&&fd, EventHandler *handler,
    const TCPConnectionInfo &info
  );
};

}} // score::net
