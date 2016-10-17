#pragma once
#include "score/exceptions/ScoreError.h"
#include "score/macros.h"
#include "score/Try.h"

namespace score { namespace posix {

class FileDescriptor {
 public:
  SCORE_DECLARE_EXCEPTION(Invalid, score::exceptions::ScoreError);
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
  Try<int> get();
  Try<int> release();
  FileDescriptor& operator=(FileDescriptor&& other);
  void maybeClose();
  ~FileDescriptor();
  static FileDescriptor takeOwnership(int fd);
};

}} // score::posix
