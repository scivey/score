#pragma once
#include <string>
#include "score/exceptions/exceptions.h"

namespace score { namespace encodings {

class LibCharsetError: public score::exceptions::BaseError {
 public:
  template<typename T>
  LibCharsetError(const T& msg): score::exceptions::BaseError(msg) {}
};

class LibCharsetDetectorHandle {
 protected:
  void *handle_ {nullptr};
  bool closed_ {false};
  LibCharsetDetectorHandle(void *handle);
  LibCharsetDetectorHandle(const LibCharsetDetectorHandle& other) = delete;
  LibCharsetDetectorHandle& operator=(const LibCharsetDetectorHandle& other) = delete;
 public:
  static LibCharsetDetectorHandle create();
  LibCharsetDetectorHandle(LibCharsetDetectorHandle&&);
  LibCharsetDetectorHandle& operator=(LibCharsetDetectorHandle&&);
  const char* csdClose();

  bool consider(const char *buff, size_t buffLen);

  template<typename TString>
  bool consider(const TString &aString) {
    return consider(aString.data(), aString.size());
  }

  bool isValid() const;
  bool isClosed() const;
  ~LibCharsetDetectorHandle();
};


}} // score::encodings