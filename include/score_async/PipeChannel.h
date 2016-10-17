#pragma once

#include <memory>
#include "score/exceptions/PosixError.h"

namespace score { namespace async {


class PipeChannel {
 public:
  using fd_t = int;
  struct ReadFD {
    fd_t fd;
    ReadFD(){}
    ReadFD(fd_t fd): fd(fd){}
  };
  struct WriteFD {
    fd_t fd;
    WriteFD(){}
    WriteFD(fd_t fd): fd(fd){}
  };
  struct SafePipe {
    ReadFD readEnd;
    WriteFD writeEnd;
    SafePipe(){}
    SafePipe(ReadFD&& rfd, WriteFD&& wfd):
      readEnd(rfd), writeEnd(wfd){}

    static SafePipe create() {
      fd_t fds[2];
      SCORE_MAKE_POSIX_CALL(pipe2(fds, O_CLOEXEC | O_NONBLOCK));
      SafePipe result;
      return SafePipe(ReadFD(fds[0]), WriteFD(fds[1]));
    }
  };
  struct ChannelFDs {
    ReadFD readEnd;
    WriteFD writeEnd;
  };

  class ChannelHandle {
    ChannelFDs fds_;
    friend class PipeChannel;
   public:
    ChannelHandle(){}
    ChannelHandle(ChannelFDs&& fds): fds_(std::forward<ChannelFDs>(fds)) {}
    fd_t getReadFD() {
      return fds_.readEnd.fd;
    }
    fd_t getWriteFD() {
      return fds_.writeEnd.fd;
    }
    ssize_t write(const void *buff, size_t count) {
      return ::write(getWriteFD(), buff, count);
    }

    template<typename TString, typename = decltype(std::declval<TString>().c_str())>
    ssize_t write(const TString &aStr) {
      return ::write(getWriteFD(), aStr.c_str(), aStr.size());
    }

    ssize_t read(void *buff, size_t count) {
      return ::read(getReadFD(), buff, count);
    }
  };

 protected:
  std::unique_ptr<ChannelHandle> first_ {nullptr};
  std::unique_ptr<ChannelHandle> second_ {nullptr};
  PipeChannel(){}
 public:
  static PipeChannel create() {
    PipeChannel instance;
    auto pipe1 = SafePipe::create();
    auto pipe2 = SafePipe::create();
    instance.first_.reset(new ChannelHandle);
    instance.second_.reset(new ChannelHandle);
    instance.first_->fds_.writeEnd = std::move(pipe1.writeEnd);
    instance.first_->fds_.readEnd = std::move(pipe2.readEnd);
    instance.second_->fds_.writeEnd = std::move(pipe2.writeEnd);
    instance.second_->fds_.readEnd = std::move(pipe1.readEnd);
    return instance;
  }
  std::unique_ptr<ChannelHandle>& getFirst() {
    return first_;
  }
  std::unique_ptr<ChannelHandle>& getSecond() {
    return second_;
  }
  std::unique_ptr<ChannelHandle>&& moveFirst() {
    return std::move(first_);
  }
  std::unique_ptr<ChannelHandle>&& moveSecond() {
    return std::move(second_);
  }
  void dismissFirst() {
    first_.release();
  }
  void dismissSecond() {
    second_.release();
  }
};

}} // evs::events
