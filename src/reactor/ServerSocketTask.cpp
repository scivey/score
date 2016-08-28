#include "aliens/reactor/TCPSocket.h"
#include "aliens/reactor/EpollReactor.h"
#include "aliens/reactor/ServerSocketTask.h"

#include <glog/logging.h>
namespace aliens { namespace reactor {

void ServerSocketTask::doRead() {
  for (;;) {
    char buff[kReadBuffSize];
    ssize_t count = read(sock_.getFdNo(), buff, kReadBuffSize);
    if (count == -1) {
      if (errno == EAGAIN) {
        break;
      }
    } else if (count == 0) {
      onEOF();
      break;
    } else {
      onRead(buff, count);
    }
  }
}

ServerSocketTask::ServerSocketTask(TCPSocket &&sock)
  : sock_(std::forward<TCPSocket>(sock)) {}

void ServerSocketTask::onReadable() {
  doRead();
}

void ServerSocketTask::onWritable() {
  VLOG(50) << "onWritable!";
}

void ServerSocketTask::onError() {
  VLOG(50) << "ServerSocketTask onError";
}

void ServerSocketTask::onRead(char* buff, ssize_t buffLen) {
  VLOG(50) << "onRead! [" << buffLen << "] : '" << buff << "'";
}

void ServerSocketTask::onEOF() {
  VLOG(50) << "onEOF!";
}

int ServerSocketTask::getFd() {
  return sock_.getFdNo();
}

}} // aliens::reactor
