#pragma once
#include <string>
#include <map>
#include <memory>
#include <event2/event.h>
#include "score/curl/detail/AsyncSocketContext.h"
#include "score/curl/detail/CurlMultiHandle.h"
#include "score/curl/detail/AsyncRequestContext.h"

namespace score { namespace curl { namespace detail {

template<typename TCurlAPI>
void AsyncCurlerContext<TCurlAPI>::libeventEventCallback(int fd, short kind, void *pdata) {
  auto ctx = (AsyncCurlerContext*) pdata;
  ctx->onLibeventEvent(fd, kind);
}

template<typename TCurlAPI>
void AsyncCurlerContext<TCurlAPI>::onLibeventEvent(int fd, short kind) {
  int action =
    (kind & EV_READ ? CURL_CSELECT_IN : 0) |
    (kind & EV_WRITE ? CURL_CSELECT_OUT : 0);
  auto rc = curl_multi_socket_action(
    multiHandle_.getCurlHandle(), fd, action, &stillRunning_
  );
  CHECK(rc == CURLM_OK);
  checkCompletions();
  if (stillRunning_ <= 0) {
    if (evtimer_pending(timerEvent_, nullptr)) {
      evtimer_del(timerEvent_);
      timerEvent_ = nullptr;
    }
  }
}

template<typename TCurlAPI>
int AsyncCurlerContext<TCurlAPI>::curlMultiTimerCallback(CURLM*, long timeoutMsec, void *pdata) {
  auto ctx = (AsyncCurlerContext*) pdata;
  ctx->onCurlMultiTimer(timeoutMsec);
  return 0;
}

template<typename TCurlAPI>
void AsyncCurlerContext<TCurlAPI>::onCurlMultiTimer(long timeoutMsec) {
  struct timeval timeout;
  timeout.tv_sec = timeoutMsec / 1000;
  timeout.tv_usec = (timeoutMsec % 1000) * 1000;
  evtimer_add(timerEvent_, &timeout);
}

template<typename TCurlAPI>
void AsyncCurlerContext<TCurlAPI>::libeventTimerCallback(int fd, short kind, void *pdata) {
  auto ctx = (AsyncCurlerContext*) pdata;
  ctx->onLibeventTimer(fd, kind);
}

template<typename TCurlAPI>
void AsyncCurlerContext<TCurlAPI>::onLibeventTimer(int fd, short kind) {
  auto rc = TCurlAPI::multi_socket_action(
    multiHandle_.getCurlHandle(), CURL_SOCKET_TIMEOUT, 0, &stillRunning_
  );
  CHECK(rc == CURLM_OK);
  checkCompletions();
}



template<typename TCurlAPI>
void AsyncCurlerContext<TCurlAPI>::updateSocket(AsyncSocketContext<TCurlAPI> *sockCtx,
    curl_socket_t sockFd, CURL *easy, int action) {
  sockCtx->update(sockFd, easy, action, this);
}

template<typename TCurlAPI>
void AsyncCurlerContext<TCurlAPI>::addSocket(curl_socket_t sock, CURL *easy, int action) {
  LOG(INFO) << "addSocket";
  auto sockCtx = socket_ctx_t::createNew(sock,
    CurlEasyHandle<TCurlAPI>::create(easy),
    action, this
  );
  curl_multi_assign(multiHandle_.getCurlHandle(), sock, (void*) this);
}

template<typename TCurlAPI>
void AsyncCurlerContext<TCurlAPI>::curlMultiSocketCallback(CURL *easy,
  curl_socket_t sock, int what, void *globalPtr, void *sockPtr) {
  LOG(INFO) << "curlMultiSocketCallback";
  auto ctx = (AsyncCurlerContext*) globalPtr;
  auto sockCtx = (socket_ctx_t*) sockPtr;
  ctx->onCurlMultiSocket(easy, sock, what, sockCtx);
}

template<typename TCurlAPI>
void AsyncCurlerContext<TCurlAPI>::onCurlMultiSocket(CURL *easy,
  curl_socket_t sock, int what, AsyncSocketContext<TCurlAPI> *sockCtx) {
  if (what == CURL_POLL_REMOVE) {
    if (sockCtx) {
      LOG(INFO) << "DELETE";
      // delete sockCtx;
    }
  } else {
    if (!sockCtx) {
      // allocate and register new socket
      addSocket(sock, easy, what);
    } else {
      updateSocket(sockCtx, sock, easy, what);
    }
  }
}

template<typename TCurlAPI>
void AsyncCurlerContext<TCurlAPI>::checkCompletions() {
  CURLMsg *msg;
  int nRemaining {0};
  CURL *easy;
  CURLcode res;
  char *effectiveUrl;
  void *easyData {nullptr};
  while ((msg = TCurlAPI::multi_info_read(multiHandle_.getCurlHandle(), &nRemaining))) {
    if (msg->msg == CURLMSG_DONE) {
      easy = msg->easy_handle;
      res = msg->data.result;
      curl_easy_getinfo(easy, CURLINFO_PRIVATE, &easyData);
      curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &effectiveUrl);
      multiHandle_.removeEasyHandle(easy);
      auto reqCtx = (AsyncRequestContext<TCurlAPI>*) easyData;
      reqCtx->triggerFinished();
      delete reqCtx;
    }
  }
}

template<typename TCurlAPI>
AsyncCurlerContext<TCurlAPI>::~AsyncCurlerContext() {
  if (timerEvent_) {
    event_free(timerEvent_);
    timerEvent_ = nullptr;
  }
}

template<typename TCurlAPI>
AsyncCurlerContext<TCurlAPI>* AsyncCurlerContext<TCurlAPI>::createNew(event_base *evBase) {
  auto instance = new AsyncCurlerContext(multi_handle_t::create());
  instance->libEventBase_ = evBase;
  instance->timerEvent_ = evtimer_new(
    instance->libEventBase_,
    AsyncCurlerContext::libeventTimerCallback,
    (void*) instance
  );
  instance->multiHandle_.setOpt(CURLMOPT_SOCKETFUNCTION, curlMultiSocketCallback);
  instance->multiHandle_.setOpt(CURLMOPT_SOCKETDATA, (void*) instance);
  instance->multiHandle_.setOpt(CURLMOPT_TIMERFUNCTION, curlMultiTimerCallback);
  instance->multiHandle_.setOpt(CURLMOPT_TIMERDATA, (void*) instance);
  return instance;
}


}}} // score::curl::detail
