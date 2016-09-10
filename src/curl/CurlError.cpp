#include "score/curl/CurlError.h"
#include <sstream>

namespace score { namespace curl {

CurlMultiError CurlMultiError::fromCode(CURLMcode code) {
  std::ostringstream msg;
  msg << "CurlMultiError [" << code << "] : '" << curl_multi_strerror(code) << "'";
  return CurlMultiError(msg.str());
}

CurlEasyError CurlEasyError::fromCode(CURLcode code) {
  std::ostringstream msg;
  msg << "CURLError [" << code << "] : '" << curl_easy_strerror(code) << "'";
  return CurlEasyError(msg.str());
}


}} // score::curl
