#pragma once

#include <string>

namespace score { namespace io {

struct base64 {
  static std::string encode(const std::string &plainText);
  static std::string decode(const std::string &encodedText);
};

}} // score::io
