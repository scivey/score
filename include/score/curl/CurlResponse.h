#pragma once
#include <vector>
#include <string>

namespace score { namespace curl {

struct CurlResponse {
  std::vector<std::string> rawHeaders;
  std::string body;
};

}} // score::curl
