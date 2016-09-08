#pragma once
#include "score/exceptions/exceptions.h"
#include <glog/logging.h>
#include <string>
#include <sstream>

namespace score { namespace encodings {

class IConvException: public score::exceptions::BaseError {
 public:
  template<typename T>
  IConvException(const T &msg): BaseError(msg) {}

  static IConvException fromErrno(int);
  static IConvException fromErrno(const std::string &prefix, int);
  static IConvException fromErrno(int err, int lineNo, const std::string &fileName);
};


#define THROW_ICONV_ERR(err) \
  throw IConvException::fromErrno(err, __LINE__, __FILE__)

}} // expr2::encodings