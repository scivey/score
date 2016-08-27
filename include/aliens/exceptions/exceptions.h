#pragma once
#include <stdexcept>

namespace aliens { namespace exceptions {

class BaseError: public std::runtime_error {
 public:
  template<typename T>
  BaseError(const T& msg): std::runtime_error(msg){}
};


}} // aliens::exceptions
