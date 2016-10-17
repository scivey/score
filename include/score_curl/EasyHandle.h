#pragma once

#include <stdexcept>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <glog/logging.h>
#include "score/macros.h"

namespace score { namespace curl {

class EasyHandle {
 protected:
  CURL *easy_ {nullptr};
  SCORE_DISABLE_COPY_AND_ASSIGN(EasyHandle);
  EasyHandle(CURL *easy);
 public:
  EasyHandle();
  EasyHandle(EasyHandle&& other);
  EasyHandle& operator=(EasyHandle&& other);
  CURL* get();
  CURL* operator->();
  bool good() const;
  operator bool() const;
  static EasyHandle create();
  static EasyHandle* createNew();
  static EasyHandle takeOwnership(CURL *easy);
  ~EasyHandle();
};

}} // score::curl
