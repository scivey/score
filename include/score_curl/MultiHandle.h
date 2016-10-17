#pragma once

#include <stdexcept>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <glog/logging.h>
#include "score/Try.h"
#include "score/Unit.h"
#include "score_curl/Easy.h"

namespace score { namespace curl {

class MultiHandle {
 protected:
  CURLM *multi_ {nullptr};
  SCORE_DISABLE_COPY_AND_ASSIGN(MultiHandle);
  MultiHandle(CURLM *multi);
 public:
  MultiHandle();
  MultiHandle(MultiHandle&& other);
  MultiHandle& operator=(MultiHandle&& other);
  CURLM* get();
  CURLM* operator->();
  void addHandle(CURL *handle);
  bool good() const;
  operator bool() const;
  static MultiHandle create();
  static MultiHandle* createNew();
  static MultiHandle takeOwnership(CURLM *multi);
  ~MultiHandle();
};



}} // score::curl
