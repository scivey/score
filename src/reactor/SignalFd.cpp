#include "aliens/reactor/SignalFd.h"
#include <sys/signalfd.h>
#include <signal.h>

#include <atomic>
#include <memory>
#include <cstdio>
#include <glog/logging.h>
#include "aliens/exceptions/macros.h"

namespace aliens { namespace reactor {


SignalFd::SignalFd(FileDescriptor &&desc, SignalFd::EventHandler *handler)
  : fd_(std::forward<FileDescriptor>(desc)),
    handler_(handler) {
  epollTask_.setParent(this);
}

void SignalFd::stop() {
  fd_.close();
}

void SignalFd::onReadable() {
  struct signalfd_siginfo fdsi;
  CHECK(read(getFdNo(), &fdsi, sizeof(fdsi)) == sizeof(fdsi));
  handler_->onSignal(fdsi.ssi_signo);
}

void SignalFd::onWritable() {
  LOG(INFO) << "SignalFd::onWritable";
}

void SignalFd::onError() {
  LOG(INFO) << "SignalFd::onError";
}

SignalFd SignalFd::create(SignalFd::EventHandler *handler) {
  sigset_t mask;
  int sfd;
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  ALIENS_CHECK_SYSCALL2(
    sigprocmask(SIG_BLOCK, &mask, nullptr),
    "sigprocmask()"
  );
  sfd = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
  ALIENS_CHECK_SYSCALL2(sfd, "signalfd()");
  return SignalFd(FileDescriptor::fromIntExcept(sfd), handler);
}

std::shared_ptr<SignalFd> SignalFd::createShared(
    SignalFd::EventHandler *handler) {
  return std::shared_ptr<SignalFd>(new SignalFd(create(
    handler
  )));
}


SignalFd::EpollTask* SignalFd::getEpollTask() {
  return &epollTask_;
}

int SignalFd::getFdNo() const {
  return fd_.getFdNo();
}


}} // aliens::reactor
