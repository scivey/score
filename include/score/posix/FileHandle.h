#pragma once

#include <functional>
#include <memory>

namespace score { namespace posix {

namespace detail {
void closeFile(FILE*);
}

class FileHandle {
 public:
  using destructor_t = std::function<void(FILE*)>;
  using file_handle_ptr = std::unique_ptr<FILE, destructor_t>;
 protected:
  file_handle_ptr handle_ {nullptr};
  FileHandle(FILE* fPtr);
 public:
  bool good() const;
  operator bool() const;
  FILE* get();
  void maybeClose();
  static FileHandle takeOwnership(FILE* fPtr);
  static FileHandle fdOpen(int fd, const char* modeStr = "r");
};


}} // score::posix
