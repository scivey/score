#pragma once
#include "aliens/reactor/TCPSocket.h"
#include "aliens/reactor/EpollReactor.h"

namespace aliens { namespace reactor {

class ServerSocketTask: public EpollReactor::Task {
 protected:
  TCPSocket sock_;
  const size_t kReadBuffSize = 512;
  void doRead();
 public:
  ServerSocketTask(TCPSocket &&sock);
  virtual void onReadable() override;
  virtual void onWritable() override;
  virtual void onError() override;
  virtual void onRead(char* buff, ssize_t buffLen);
  virtual void onEOF();
  int getFd() override;
};

}} // aliens::reactor
