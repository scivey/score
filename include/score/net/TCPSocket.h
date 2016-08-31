#pragma once
#include <string>
#include "score/reactor/FileDescriptor.h"
#include "score/net/SocketAddr.h"

namespace score { namespace reactor {

class TCPSocket {
 protected:
  FileDescriptor fd_;
  short localPort_ {0};
  short remotePort_ {0};
  std::string remoteHost_ {""};
  TCPSocket(FileDescriptor &&fd);
 public:
  static TCPSocket fromAccepted(FileDescriptor &&fd, const char* remoteHost, const char *remotePort);
  static TCPSocket connect(SocketAddr addr);
  static TCPSocket bindPort(short portno);
  bool valid() const;
  int getFdNo() const;
  void listen();
  void stop();
};


}} // score::reactor
