#pragma once

#include <string>

namespace aliens { namespace io {

template<typename TString = std::string>
TString trimAsciiWhitespace(const TString &aStr) {
  TString result;
  for (auto c: aStr) {
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
      continue;
    }
    if (c == '\0') {
      continue;
    }
    result.push_back(c);
  }
  return result;
}

}} // aliens::io
