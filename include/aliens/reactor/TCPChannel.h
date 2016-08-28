#pragma once
#include <glog/logging.h>
#include <glog/logging.h>
#include "aliens/reactor/FdHandlerBase.h"
#include "aliens/reactor/SocketAddr.h"
#include "aliens/reactor/ParentHaving.h"
#include "aliens/reactor/TCPConnectionInfo.h"
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

 protected:
  EventHandler *handler_ {nullptr};
  TCPConnectionInfo connInfo_;
  TCPChannel(FileDescriptor &&, EventHandler*, const TCPConnectionInfo&);
  void readSome();

 public:
  void triggerReadable();
  void triggerWritable();
  void triggerError();
  void sendBuff(io::NonOwnedBufferPtr, async::ErrBack &&errback);
  void shutdown();
  static TCPChannel fromDescriptor(
    FileDescriptor&&fd, EventHandler *handler,
    const TCPConnectionInfo &info
  );
  static TCPChannel* fromDescriptorPtr(
    FileDescriptor&&fd, EventHandler *handler,
    const TCPConnectionInfo &info
  );
  static std::shared_ptr<TCPChannel> fromDescriptorShared(
    FileDescriptor&&fd, EventHandler *handler,
    const TCPConnectionInfo &info
  );
  static std::unique_ptr<TCPChannel> fromDescriptorUnique(
    FileDescriptor&&fd, EventHandler *handler,
    const TCPConnectionInfo &info
  );
};

}} // aliens::reactor