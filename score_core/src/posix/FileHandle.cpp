#include "score/posix/FileHandle.h"
#include <memory>
#include <glog/logging.h>

namespace score { namespace posix {

namespace detail {
void closeFile(FILE *fPtr) {
  fclose(fPtr);
}
} // detail


FileHandle::FileHandle(FILE* fPtr)
  : handle_(fPtr, detail::closeFile) {}

FileHandle FileHandle::takeOwnership(FILE* fPtr) {
  return FileHandle(fPtr);
}

FileHandle FileHandle::fdOpen(int fd, const char *modeStr) {
  auto fPtr = fdopen(fd, modeStr);
  CHECK(fPtr != nullptr);
  return FileHandle::takeOwnership(fPtr);
}

FILE* FileHandle::get() {
  CHECK(!!handle_);
  return handle_.get();
}

void FileHandle::maybeClose() {
  handle_.release();
}

bool FileHandle::good() const {
  return !!handle_;
}

FileHandle::operator bool() const {
  return good();
}


}} // score::posix
