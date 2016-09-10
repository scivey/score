#include "score/curl/CurlError.h"
#include <sstream>

namespace score { namespace curl {

CurlMError CurlMError::fromCode(CURLMcode code) {
  std::ostringstream msg;
  msg << "CURLMError [" << code << "] : '" << curl_multi_strerror(code) << "'";
  return CurlMError(msg.str());
}

CurlEasyError CurlEasyError::fromCode(CURLcode code) {
  std::ostringstream msg;
  msg << "CURLError [" << code << "] : '" << curl_easy_strerror(code) << "'";
  return CurlEasyError(msg.str());
}


}} // score::curl
