#include "score/exceptions/PosixError.h"
#include "score/ExceptionWrapper.h"
#include "score/format.h"
#include <errno.h>

namespace score { namespace exceptions {


std::string formatErrno(int err, const std::string& msg) {
  return score::sformat("[{}] '{}' : '{}'", err, strerror(err), msg);
}

}}
