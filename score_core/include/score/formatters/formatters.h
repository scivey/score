#pragma once
#include <iostream>
#include <sstream>
#include <folly/Format.h>

namespace score { namespace formatters {

std::ostream& operator<<(std::ostream &oss, __uint128_t num128);

template<typename ...Types >
std::string tryFormat(const char *fmt, Types&&... args) {
  try {
    return folly::sformat(fmt, std::forward<Types>(args)...);
  } catch (const folly::BadFormatArg &ex) {
    std::ostringstream oss;
    oss << "Original message: '" << fmt << "'";
    oss << ", but caught format error: '" << ex.what() << "'";
    return oss.str();
  }
}


}} // score::formatters
