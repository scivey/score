
#include "score/curl/EasyError.h"

namespace score { namespace curl {

EasyError::EasyError(CURLcode code)
  : exceptions::ScoreError(curl_easy_strerror(code)), curlCode_(code) {}
CURLcode EasyError::curlCode() const {
  return curlCode_;
}

}} // score::curl