#include <stdexcept>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <glog/logging.h>
#include "score/macros.h"
#include "score/curl/EasyHandle.h"

namespace score { namespace curl {

EasyHandle::EasyHandle(CURL *easy): easy_(easy){}

EasyHandle::EasyHandle(){}

EasyHandle::EasyHandle(EasyHandle&& other): easy_(other.easy_) {
  other.easy_ = nullptr;
}

EasyHandle& EasyHandle::operator=(EasyHandle&& other) {
  std::swap(easy_, other.easy_);
  return *this;
}

CURL* EasyHandle::get() {
  DCHECK(good());
  return easy_;
}
CURL* EasyHandle::operator->() {
  DCHECK(good());
  return easy_;
}
bool EasyHandle::good() const {
  return !!easy_;
}
EasyHandle::operator bool() const {
  return good();
}
EasyHandle EasyHandle::create() {
  return takeOwnership(curl_easy_init());
}
EasyHandle* EasyHandle::createNew() {
  return new EasyHandle { create() };
}
EasyHandle EasyHandle::takeOwnership(CURL *easy) {
  return EasyHandle(easy);
}
EasyHandle::~EasyHandle() {
  if (easy_) {
    curl_easy_cleanup(easy_);
    easy_ = nullptr;
  }
}

}} // score::curl
