#include "score/encodings/IConvException.h"
#include <glog/logging.h>
#include <string>
#include <sstream>

namespace score { namespace encodings {

IConvException IConvException::fromErrno(int err) {
  std::ostringstream msg;
  msg << "IConvException[" << err << "] : '" << strerror(err) << "'";
  return IConvException(msg.str());
}

IConvException IConvException::fromErrno(const std::string &prefix, int err) {
  std::ostringstream msg;
  msg << "IConvException[" << err << "] { " << prefix << " } : '" <<  strerror(err) << "'";
  return IConvException(msg.str());
}


IConvException IConvException::fromErrno(int err, int lineNo, const std::string &fileName) {
  std::ostringstream msg;
  msg << "IConvException {" << fileName << ":" << lineNo << "}";
  msg << " [" << err << "] : '" << strerror(err) << "'";
  return IConvException(msg.str());
}

}} // score::encodings
