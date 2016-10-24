
#include <stdexcept>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <glog/logging.h>
#include "score/io/ByteStringPiece.h"
#include "score/util/try_helpers.h"
#include "score/Try.h"
#include "score/Unit.h"
#include "score/curl/EasyHandle.h"
#include "score/curl/Easy.h"

namespace score { namespace curl {

Easy::Easy(){}

Easy::Easy(std::unique_ptr<EasyHandle>&& handle)
  : handle_(std::move(handle)){}

Easy Easy::create() {
  return Easy(std::unique_ptr<EasyHandle> {
    EasyHandle::createNew()
  });
}

Easy* Easy::createNew() {
  return new Easy(create());
}

CURL* Easy::get() {
  return handle_->get();
}

const std::string& Easy::getURL() const {
  return url_;
}

bool Easy::good() const {
  return !!handle_;
}

Easy::operator bool() const {
  return good();
}

Try<Unit> Easy::perform() {
  auto rc = curl_easy_perform(get());
  if (rc != CURLE_OK) {
    return util::makeTryFailure<Unit, EasyError>(rc);
  }
  return util::makeTrySuccess<Unit>();
}

size_t Easy::onWrite(void *ptr, size_t sz, size_t nMembers) {
  if (sz && nMembers) {
    io::ByteStringPiece piece {(char*) ptr, nMembers};
    bodyBuffer_ << piece;
  }
  size_t total = sz * nMembers;
  return total;
}

size_t Easy::curlWriteCallback(void *ptr, size_t sz, size_t nm, void *data) {
  auto ctx = (Easy*) data;
  return ctx->onWrite(ptr, sz, nm);
}

int Easy::onProgress(double total, double now, double ult, double uln) {
  return 0;
}

size_t Easy::curlProgressCallback(void *data, double a, double b, double c, double d) {
  auto ctx = (Easy*) data;
  return ctx->onProgress(a, b, c, d);
}

void Easy::bindCallbacks() {
  setOpt(CURLOPT_PROGRESSFUNCTION, Easy::curlProgressCallback);
  setOpt(CURLOPT_PROGRESSDATA, (void*) this);
  setOpt(CURLOPT_WRITEFUNCTION, Easy::curlWriteCallback);
  setOpt(CURLOPT_PRIVATE, (void*) this);
  setOpt(CURLOPT_WRITEDATA, (void*) this);
  setOpt(CURLOPT_ERRORBUFFER, (char*) errBuffer_);
  setOpt(CURLOPT_VERBOSE, 0L);
}

void Easy::setURL(const std::string& url) {
  url_ = url;
  setOpt(CURLOPT_URL, url_.c_str());
}

std::ostringstream& Easy::getBodyBuffer() {
  return bodyBuffer_;
}

curl_socket_t Easy::getFD() {
  curl_socket_t sock {0};
  auto rc = curl_easy_getinfo(get(), CURLINFO_LASTSOCKET, &sock);
  CHECK(rc == CURLE_OK);
  return sock;
}

Easy* Easy::fromOwnedEasyHandle(CURL *ptr) {
  uintptr_t privPointer {0};
  auto rc = curl_easy_getinfo(ptr, CURLINFO_PRIVATE, &privPointer);
  CHECK(rc == CURLE_OK);
  CHECK(privPointer != 0);
  return (Easy*) privPointer;
}

void Easy::onFinished() {
  CHECK(doneCallback_);
  doneCallback_(bodyBuffer_.str());
}

}} // score::curl
