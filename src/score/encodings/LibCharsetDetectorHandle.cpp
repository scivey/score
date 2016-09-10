#include "score/encodings/LibCharsetDetectorHandle.h"
#include "score/macros.h"
#include <string>
#include "score/vendored/lgpl/charsetdetect/charsetdetect.h"

namespace score { namespace encodings {

LibCharsetDetectorHandle::LibCharsetDetectorHandle(
    LibCharsetDetectorHandle &&other)
  : handle_(other.handle_) {
  other.handle_ = nullptr;
}

LibCharsetDetectorHandle::LibCharsetDetectorHandle(void *handle)
  : handle_(handle) {}



const char* LibCharsetDetectorHandle::csdClose() {
  SDCHECK(isValid());
  auto result = csd_close(handle_);
  closed_ = true;
  return result;
}

bool LibCharsetDetectorHandle::consider(
    const char* buff, size_t buffLen) {
  SDCHECK(isValid());
  return csd_consider(handle_, buff, buffLen);
}

bool LibCharsetDetectorHandle::isClosed() const {
  return closed_;
}

bool LibCharsetDetectorHandle::isValid() const {
  return handle_ != nullptr && !isClosed();
}

LibCharsetDetectorHandle LibCharsetDetectorHandle::create() {
  auto handlePtr = csd_open();
  if (handlePtr == (csd_t) - 1) {
    throw LibCharsetError("csd_open()");
  }
  return LibCharsetDetectorHandle(handlePtr);
}

LibCharsetDetectorHandle::~LibCharsetDetectorHandle() {
  if (!closed_) {
    auto result = csdClose();
    SCORE_UNUSED(result);
  }
}

}} // score::encodings