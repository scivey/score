#pragma once

#include <stdexcept>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <glog/logging.h>
#include "score/macros.h"
#include "score/exceptions/ScoreError.h"

namespace score { namespace curl {

class EasyError : public exceptions::ScoreError {
 protected:
  CURLcode curlCode_;
 public:
  EasyError(CURLcode code);
  CURLcode curlCode() const;
};

}} // score::curl