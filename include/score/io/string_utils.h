#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace score { namespace io {


std::string trimWhitespace(const std::string& text);


template<typename TCollection>
std::string joinWith(const std::string& separator,
    const TCollection &parts) {
  std::ostringstream oss;
  size_t last = 0;
  if (parts.size() > 0) {
    last = parts.size() - 1;
  }
  size_t i = 0;
  for (const auto &part: parts) {
    oss << part;
    if (i != last) {
      oss << separator;
    }
    i++;
  }
  return oss.str();
}

bool isAsciiPunctuation(char c);


}} // score::io
