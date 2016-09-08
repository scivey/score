#pragma once
#include "score/encodings/IConvHandle.h"
#include "score/encodings/IConvException.h"

#include "score/encodings/HeapBufferHandle.h"
#include "score/macros.h"
#include <sstream>
#include <string>
#include <folly/Optional.h>
namespace score { namespace encodings {

class IConverter {
 protected:
  IConvHandle handle_;
  IConverter(IConvHandle&&);
  IConverter(const IConverter&) = delete;
  IConverter& operator=(const IConverter&) = delete;
 public:
  IConverter(IConverter&&);
  IConverter& operator=(IConverter&&);

  static IConverter create(const IConvOptions&);
  std::string convert(const std::string&);
};

}} // expr2::encodings