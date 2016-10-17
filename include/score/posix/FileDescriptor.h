#pragma once
#include "score/ScoreError.h"
#include "score/macros.h"
#include <folly/futures/Try.h>

namespace evs { namespace posix {

class FileDescriptor {
 public:
  SCORE_DECLARE_EXCEPTION(Invalid, ScoreError);
 protected:
  int fd_ {0};
  FileDescriptor(const FileDescriptor&) = delete;
  FileDescriptor& operator=(const FileDescriptor&) = delete;
  explicit FileDescriptor(int fd);
 public:
  FileDescriptor();
  FileDescriptor(FileDescriptor&& other);
  bool good() const;
  operator bool() const;
  folly::Try<int> get();
  folly::Try<int> release();
  FileDescriptor& operator=(FileDescriptor&& other);
  void maybeClose();
  ~FileDescriptor();
  static FileDescriptor takeOwnership(int fd);
};

}} // evs::posix
