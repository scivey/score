#pragma once
#include <string>
#include "aliens/reactor/FileDescriptor.h"
#include "aliens/reactor/SocketAddr.h"

namespace aliens { namespace reactor {

class AcceptSocketTask;
class ServerSocketTask;
class ClientSocketTask;

class TCPSocket {
 protected:
  FileDescriptor fd_;
  short localPort_ {0};
  short remotePort_ {0};
  std::string remoteHost_ {""};
  TCPSocket(FileDescriptor &&fd);
  int getFdNo();
 public:
  friend class AcceptSocketTask;
  friend class ServerSocketTask;
  friend class ClientSocketTask;
  static TCPSocket fromAccepted(FileDescriptor &&fd, const char* remoteHost, const char *remotePort);
  static TCPSocket connect(SocketAddr addr);
  static TCPSocket bindPort(short portno);
  bool valid() const;
  void listen();
  void stop();
};


}} // aliens::reactor
