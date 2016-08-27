#pragma once

namespace aliens { namespace reactor {

class FileDescriptor {
 protected:
  int fd_ {0};
  FileDescriptor();
  FileDescriptor(int fd);
  FileDescriptor(const FileDescriptor&) = delete;
  FileDescriptor& operator=(const FileDescriptor&) = delete;

  void maybeClose();
 public:
  static FileDescriptor fromIntExcept(int fd);
  FileDescriptor(FileDescriptor &&other);
  FileDescriptor& operator=(FileDescriptor &&other);
  int get();
  bool valid() const;
  explicit operator bool() const;
  void makeNonBlocking();
  void close();
  ~FileDescriptor();
};

}} // aliens::reactor
