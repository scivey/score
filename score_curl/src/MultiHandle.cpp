#include <stdexcept>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <glog/logging.h>
#include "score/Try.h"
#include "score/Unit.h"
#include "score_curl/Easy.h"
#include "score_curl/MultiHandle.h"

namespace score { namespace curl {

MultiHandle::MultiHandle(CURLM *multi): multi_(multi){}

MultiHandle::MultiHandle(){}

MultiHandle::MultiHandle(MultiHandle&& other): multi_(other.multi_) {
  other.multi_ = nullptr;
}

MultiHandle& MultiHandle::operator=(MultiHandle&& other) {
  std::swap(multi_, other.multi_);
  return *this;
}
CURLM* MultiHandle::get() {
  DCHECK(good());
  return multi_;
}
CURLM* MultiHandle::operator->() {
  DCHECK(good());
  return multi_;
}
void MultiHandle::addHandle(CURL *handle) {
  CHECK(curl_multi_add_handle(multi_, handle) == CURLM_OK);
}
bool MultiHandle::good() const {
  return !!multi_;
}
MultiHandle::operator bool() const {
  return good();
}
MultiHandle MultiHandle::create() {
  return takeOwnership(curl_multi_init());
}
MultiHandle* MultiHandle::createNew() {
  return new MultiHandle { create() };
}
MultiHandle MultiHandle::takeOwnership(CURLM *multi) {
  return MultiHandle(multi);
}
MultiHandle::~MultiHandle() {
  if (multi_) {
    curl_multi_cleanup(multi_);
    multi_ = nullptr;
  }
}



}} // score::curl
