#include "score/reactor/SignalFd.h"
#include "score/exceptions/macros.h"
#include <sys/signalfd.h>
#include <signal.h>

#include <atomic>
#include <memory>
#include <cstdio>
#include <glog/logging.h>

using score::posix::FileDescriptor;

namespace score { namespace reactor {


SignalFd::SignalFd(FileDescriptor &&desc, SignalFd::EventHandler *handler)
  : FdHandlerBase<SignalFd>(std::forward<FileDescriptor>(desc)),
    handler_(handler) {}

void SignalFd::onReadable() {
  struct signalfd_siginfo fdsi;
  SDCHECK(read(getFdNo(), &fdsi, sizeof(fdsi)) == sizeof(fdsi));
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
  sigaddset(&mask, SIGUSR1);
  // SCORE_CHECK_SYSCALL2(
  //   sigprocmask(SIG_BLOCK, &mask, nullptr),
  //   "sigprocmask()"
  // );
  sfd = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
  SCORE_CHECK_SYSCALL2(sfd, "signalfd()");
  return SignalFd(FileDescriptor::fromIntExcept(sfd), handler);
}

SignalFd* SignalFd::createPtr(
    SignalFd::EventHandler *handler) {
  return new SignalFd(create(
    handler
  ));
}


}} // score::reactor