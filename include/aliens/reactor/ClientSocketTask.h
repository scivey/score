#pragma once
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/TCPSocket.h"
#include "aliens/FixedBuffer.h"
#include "aliens/async/ErrBack.h"

#include <stdexcept>

namespace aliens { namespace reactor {

class ClientSocketTask: public EpollReactor::Task {
 public:
  class EventHandler {
   protected:
    ClientSocketTask *task_ {nullptr};
    void setTask(ClientSocketTask *task);
   public:
    friend class ClientSocketTask;
    virtual ClientSocketTask* getTask();
    virtual void onConnectSuccess() = 0;
    virtual void onConnectError(const std::exception&) = 0;
    virtual void onWritable() = 0;
    virtual void onReadable() = 0;
    virtual void write(std::unique_ptr<Buffer> buff, async::ErrBack &&cb);
    virtual void readInto(std::unique_ptr<Buffer> buff, async::ErrBack &&cb);
  };
  friend class EventHandler;
 protected:
  TCPSocket sock_;
  EventHandler *handler_ {nullptr};
  void readInto(std::unique_ptr<Buffer> buff, async::ErrBack &&cb);
  void write(std::unique_ptr<Buffer> buff, async::ErrBack &&cb);
 public:
  ClientSocketTask(TCPSocket &&sock, EventHandler *handler);
  virtual void onReadable() override;
  virtual void onWritable() override;
  virtual void onError() override;
  int getFd() override;
};

}} // aliens::reactor
