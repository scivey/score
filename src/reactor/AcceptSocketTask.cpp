#include "aliens/reactor/AcceptSocketTask.h"
#include "aliens/reactor/ServerSocketTask.h"
#include <glog/logging.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

namespace aliens { namespace reactor {

void AcceptSocketTask::doAccept() {
  for (;;) {
    struct sockaddr inAddr;
    socklen_t inLen;
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    inLen = sizeof inAddr;
    int inFd = accept(getFd(), &inAddr, &inLen);
    if (inFd == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // all incoming connections processed.
        break;
      } else {
        onAcceptError(errno);
        break;
      }
    }
    int status = getnameinfo(
      &inAddr, inLen,
      hbuf, sizeof hbuf,
      sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV);
    if (status == 0) {
      onAcceptSuccess(inFd, hbuf, sbuf);
    } else {
      onAcceptError(errno);
    }
  }
}

AcceptSocketTask::AcceptSocketTask(TCPSocket &&sock)
  : sock_(std::forward<TCPSocket>(sock)) {}

void AcceptSocketTask::onEvent() {
  LOG(INFO) << "onEvent! Accepting....";
  doAccept();
}

void AcceptSocketTask::onError() {
  LOG(INFO) << "onError!";
}

int AcceptSocketTask::getFd() {
  return sock_.fd_.get();
}

TCPSocket& AcceptSocketTask::getSocket() {
  return sock_;
}

void AcceptSocketTask::onAcceptSuccess(int inFd, const char *host, const char *port) {
  LOG(INFO) << "onAcceptSuccess : [" << inFd << "] " << host << ":" << port;
  auto fd = FileDescriptor::fromIntExcept(inFd);
  fd.makeNonBlocking();
  auto sock = TCPSocket::fromAccepted(
    std::move(fd), host, port
  );
  auto newTask = new ServerSocketTask(std::move(sock));
  LOG(INFO) << "made new task.";
  getReactor()->addTask(newTask);
}

void AcceptSocketTask::onAcceptError(int err) {
  LOG(INFO) << "onAcceptError : " << strerror(err);
}

}} // aliens::reactor
