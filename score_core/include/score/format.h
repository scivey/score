#pragma once

#include <string>
#include <sstream>
#include "score/vendored/fmt/format.h"

namespace score {

template<typename ...Types>
std::string format(Types&&... args) {
  return score::vendored::fmt::format(std::forward<Types>(args)...);
}

template<typename ...Types>
std::string sformat(Types&&... args) {
  return score::vendored::fmt::format(std::forward<Types>(args)...);
}

template<typename ...Types >
std::string tryFormat(const char *formatStr, Types&&... args) {
  try {
    return format(formatStr, std::forward<Types>(args)...);
  } catch (const score::vendored::fmt::FormatError& ex) {
    std::ostringstream oss;
    oss << "Original message: '" << formatStr << "'";
    oss << ", but caught format error: '" << ex.what() << "'";
    return oss.str();
  }
}


} // score
