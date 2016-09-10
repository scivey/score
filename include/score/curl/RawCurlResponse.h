#pragma once
#include <vector>
#include <string>
#include <sstream>

namespace score { namespace curl {

struct RawCurlResponse {
  std::vector<std::string> rawHeaders;
  std::ostringstream bodyBuffer;
};

}} // score::curl
