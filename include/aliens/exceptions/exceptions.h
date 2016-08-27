#pragma once
#include <stdexcept>

namespace aliens { namespace exceptions {

class BaseError: public std::runtime_error {
 public:
  template<typename T>
  BaseError(const T& msg): std::runtime_error(msg){}
};

class SystemError: public BaseError {
 protected:
  int errno_ {0};
 public:
  SystemError(int err, const std::string &msg);
  SystemError(int err);
  int getErrno() const;
  static SystemError fromErrno(int err, const std::string &msg);
};


}} // aliens::exceptions
