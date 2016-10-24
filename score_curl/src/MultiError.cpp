#include "score/curl/MultiError.h"

#include <stdexcept>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <glog/logging.h>
#include "score/macros.h"
#include "score/exceptions/ScoreError.h"

namespace score { namespace curl {

MultiError::MultiError(CURLMcode code)
  : exceptions::ScoreError(curl_multi_strerror(code)), curlCode_(code) {}

CURLMcode MultiError::curlCode() const {
  return curlCode_;
}

}} // score::curl