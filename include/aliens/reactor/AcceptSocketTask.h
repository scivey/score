#pragma once
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/TCPSocket.h"

namespace aliens { namespace reactor {

class AcceptSocketTask: public EpollReactor::Task {
 protected:
  TCPSocket sock_;
  void doAccept();
 public:
  AcceptSocketTask(TCPSocket &&sock);
  void onWritable() override;
  void onReadable() override;
  void onError() override;
  int getFd() override;
  TCPSocket& getSocket();
  virtual void onAcceptSuccess(int inFd, const char *host, const char *port);
  virtual void onAcceptError(int err);
};

}} // aliens::reactor
