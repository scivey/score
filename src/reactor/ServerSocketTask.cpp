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
  LOG(INFO) << "ServerSocketTask onEvent";
  doRead();
}

void ServerSocketTask::onWritable() {
  LOG(INFO) << "onWritable!";
}

void ServerSocketTask::onError() {
  LOG(INFO) << "ServerSocketTask onError";
}

void ServerSocketTask::onRead(char* buff, ssize_t buffLen) {
  LOG(INFO) << "onRead! [" << buffLen << "] : '" << buff << "'";
}

void ServerSocketTask::onEOF() {
  LOG(INFO) << "onEOF!";
}

int ServerSocketTask::getFd() {
  return sock_.fd_.get();
}

}} // aliens::reactor
