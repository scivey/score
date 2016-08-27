#include "aliens/exceptions/exceptions.h"
#include <stdexcept>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <sys/types.h>

namespace aliens { namespace exceptions {

SystemError::SystemError(int err, const std::string &msg)
  : BaseError(msg), errno_(err) {}
SystemError::SystemError(int err)
  : BaseError(strerror(err)), errno_(err){}
int SystemError::getErrno() const {
  return errno_;
}

SystemError SystemError::fromErrno(int err, const std::string &msg) {
  std::ostringstream oss;
  oss << "ERR [" << err << ":'" << strerror(err) << "'] : '" << msg << "'";
  return SystemError(err, oss.str());
}

}} // aliens::exceptions
