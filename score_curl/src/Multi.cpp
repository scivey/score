#include "score_curl/Multi.h"

namespace score { namespace curl {

Multi::Multi(){}

Multi::Multi(std::unique_ptr<MultiHandle>&& handle)
  : handle_(std::move(handle)){}

Multi Multi::create() {
  return Multi(std::unique_ptr<MultiHandle> {
    MultiHandle::createNew()
  });
}

Multi* Multi::createNew() {
  return new Multi(create());
}

CURLM* Multi::get() {
  return handle_->get();
}

bool Multi::good() const {
  return !!handle_;
}

Multi::operator bool() const {
  return good();
}

void Multi::addHandle(Easy& handle) {
  handle_->addHandle(handle.get());
}

void Multi::addHandle(Easy *handle) {
  handle_->addHandle(handle->get());
}

int Multi::perform() {
  int nRunning = 0;
  auto rc = curl_multi_perform(get(), &nRunning);
  CHECK(rc == CURLM_OK);
  return nRunning;
}

long Multi::getTimeout() {
  long result {0};
  auto rc = curl_multi_timeout(get(), &result);
  CHECK(rc == CURLM_OK);
  return result;
}

void Multi::bindCallbacks() {
  setOpt(CURLMOPT_SOCKETFUNCTION, Multi::curlSocketCallback);
  setOpt(CURLMOPT_SOCKETDATA, (void*) this);
  setOpt(CURLMOPT_TIMERFUNCTION, Multi::curlTimerCallback);
  setOpt(CURLMOPT_TIMERDATA, (void*) this);
}


int Multi::onCurlTimer(long tm) {
  if (timerHandler_) {
    return timerHandler_(tm);
  } else {
    LOG(INFO) << "no timer handler!";
    return 0;
  }
}

int Multi::curlTimerCallback(CURLM *multi, long tm, void *data) {
  auto ctx = (Multi*) data;
  return ctx->onCurlTimer(tm);
}


int Multi::onCurlSocket(CURL *easy, curl_socket_t sockFd, int what, void *sockPtr) {
  if (socketHandler_) {
    return socketHandler_(easy, sockFd, what, sockPtr);
  } else {
    LOG(INFO) << "no socket handler!";
    return 0;
  }
}

int Multi::curlSocketCallback(CURL *easy, curl_socket_t s, int what, void *userp, void *socketp) {
  auto ctx = (Multi*) userp;
  return ctx->onCurlSocket(easy, s, what, socketp);
}


}} // score::curl
