#pragma once

#include <stdexcept>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <glog/logging.h>
#include "score/macros.h"
#include "score/exceptions/ScoreError.h"

namespace score { namespace curl {


class MultiError : public exceptions::ScoreError {
 protected:
  CURLMcode curlCode_;
 public:
  MultiError(CURLMcode code);
  CURLMcode curlCode() const;
};

}} // score::curl