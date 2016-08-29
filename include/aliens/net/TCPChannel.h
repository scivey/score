#pragma once
#include <glog/logging.h>
#include <glog/logging.h>
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/net/SocketAddr.h"
#include "aliens/misc/ParentHaving.h"
#include "aliens/net/TCPConnectionInfo.h"
#include "aliens/async/ErrBack.h"
#include "aliens/io/NonOwnedBufferPtr.h"

namespace aliens { namespace net {

class TCPChannel : public reactor::FdHandlerBase<TCPChannel> {
 public:
  class EventHandler: public misc::ParentHaving<TCPChannel> {
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

 public:
  void triggerReadable();
  void triggerWritable();
  void triggerError();
  void sendBuff(io::NonOwnedBufferPtr, async::ErrBack &&errback);
  void shutdown();
  static TCPChannel fromDescriptor(
    posix::FileDescriptor&&fd, EventHandler *handler,
    const TCPConnectionInfo &info
  );
  static TCPChannel* fromDescriptorPtr(
    posix::FileDescriptor&&fd, EventHandler *handler,
    const TCPConnectionInfo &info
  );
  static std::shared_ptr<TCPChannel> fromDescriptorShared(
    posix::FileDescriptor&&fd, EventHandler *handler,
    const TCPConnectionInfo &info
  );
  static std::unique_ptr<TCPChannel> fromDescriptorUnique(
    posix::FileDescriptor&&fd, EventHandler *handler,
    const TCPConnectionInfo &info
  );
};

}} // aliens::net
