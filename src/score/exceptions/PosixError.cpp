#include "score/exceptions/PosixError.h"
#include "score/ExceptionWrapper.h"
#include <folly/Format.h>
#include <errno.h>

namespace score { namespace exceptions {


std::string formatErrno(int err, const std::string& msg) {
  return folly::sformat("[{}] '{}' : '{}'", err, strerror(err), msg);
}

}}
