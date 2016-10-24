#pragma once
#include "score/encodings/HeapBufferHandle.h"
#include "score/encodings/Encoding.h"
#include "score/encodings/IConvOptions.h"
#include "score/encodings/IConvException.h"

#include <memory>
#include <exception>
#include "score/macros.h"

namespace score { namespace encodings {

struct IConvFeedParams {
  char *inBuff {nullptr};
  size_t inBytesLeft {0};
  char *outBuff {nullptr};
  size_t outBytesLeft {0};
};

class IConvHandle {
 protected:
  void* handle_ {nullptr};
  IConvOptions options_;
  IConvHandle(const IConvOptions&);
  IConvHandle(void *handle, const IConvOptions&);
  IConvHandle(const IConvHandle&) = delete;
  IConvHandle& operator=(const IConvHandle&) = delete;
  void maybeClose();
 public:
  IConvHandle(IConvHandle&&);
  IConvHandle& operator=(IConvHandle&&);
  static IConvHandle create(const IConvOptions&);
  bool isValid() const;
  const IConvOptions& getOptions() const;
  void feed(IConvFeedParams* params);
  ~IConvHandle();
};



}} // score::encodings

