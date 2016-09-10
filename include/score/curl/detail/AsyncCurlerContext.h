#pragma once
#include <string>
#include <map>
#include <memory>
#include <event2/event.h>
#include "score/curl/detail/CurlMultiHandle.h"
#include "score/curl/detail/CurlEasyHandle.h"

namespace score { namespace curl { namespace detail {

template<typename TCurlAPI>
class AsyncSocketContext;


template<typename TCurlAPI>
class AsyncCurlerContext {
 public:
  using curl_api_policy = TCurlAPI;
  using multi_handle_t = CurlMultiHandle<TCurlAPI>;
  using easy_handle_t = CurlEasyHandle<TCurlAPI>;
  using socket_ctx_t = AsyncSocketContext<TCurlAPI>;

 protected:
  multi_handle_t multiHandle_;
  struct event_base *libEventBase_ {nullptr};
  struct event *timerEvent_ {nullptr};
  size_t idCounter_ {0};
  int stillRunning_ {0};
  AsyncCurlerContext(multi_handle_t &&multiHandle)
    : multiHandle_(std::forward<multi_handle_t>(multiHandle)) {}
 public:
  event_base* getLibeventBase() const {
    return libEventBase_;
  }
  static void libeventTimerCallback(int, short, void*);
  static int curlMultiTimerCallback(CURLM*, long, void*);
  static void curlMultiSocketCallback(
    CURL*, curl_socket_t, int what,
    void *globalPtr, void *sockPtr
  );
  static void libeventEventCallback(int, short, void*);
  static AsyncCurlerContext* createNew(event_base *evBase);
  size_t getNewId() {
    return idCounter_++;
  }
  CURLM* getCurlHandle() {
    return multiHandle_.getCurlHandle();
  }
  void updateSocket(socket_ctx_t*, curl_socket_t, CURL*, int action);
  void addSocket(curl_socket_t, CURL*, int action);
  void onCurlMultiTimer(long timeoutMsec);
  void onLibeventTimer(int fd, short kind);
  void onLibeventEvent(int fd, short kind);
  void onCurlMultiSocket(
    CURL*, curl_socket_t,
    int what, AsyncSocketContext<TCurlAPI>*
  );
  void checkCompletions();
  ~AsyncCurlerContext();
};

}}} // score::curl::detail