
#include <memory>
#include <string>
#include "score/async/EventContext.h"
#include "score/async/wheel/HTimerWheel.h"

#include "score/curl/EventCurler.h"
#include "score/curl/Easy.h"
#include "score/curl/Multi.h"
#include "score/util/misc.h"

namespace score { namespace curl {

void EventCurler::initTimer() {
  ctx_->getWheel()->addRepeating([this]() {
    update();
  }, std::chrono::milliseconds{50});
}

void EventCurler::bindToMulti() {
  multi_->bindCallbacks();
  multi_->setSocketHandler([](CURL *easy, curl_socket_t sockFd, int what, void *pdata) {
    return 0;
  });
  multi_->setTimerHandler([this](long timeoutMsec) {
    ctx_->getWheel()->addOneShot([this]() {
      update();
    }, std::chrono::milliseconds(timeoutMsec));
    return 0;
  });
}
EventCurler::EventCurler(ctx_t *ctx, multi_ptr_t&& multi)
  : ctx_(ctx), multi_(std::move(multi)) {}

void EventCurler::update() {
  auto nPending = perform();
  if (nPending < nPending_) {
    int nRemaining = 0;
    CURLMsg *msg {nullptr};
    do {
      msg = curl_multi_info_read(multi_->get(), &nRemaining);
      if (!!msg && msg->msg == CURLMSG_DONE) {
        auto easyHandle = msg->easy_handle;
        auto original = Easy::fromOwnedEasyHandle(easyHandle);
        original->onFinished();
        delete original;
      }
    } while (!!msg && nRemaining > 0);
  }
  nPending_ = nPending;
}

EventCurler* EventCurler::createNew(ctx_t *ctx) {
  auto curler = new EventCurler(ctx, multi_ptr_t {multi_t::createNew()});
  curler->initTimer();
  curler->bindToMulti();
  return curler;
}

int EventCurler::perform() {
  return multi_->perform();
}

void EventCurler::addHandle(Easy *handle) {
  handle->bindCallbacks();
  multi_->addHandle(handle);
}

void EventCurler::getURL(const string_t& url, get_cb_t cb) {
  auto easy = Easy::createNew();
  easy->setURL(url);
  easy->setDoneCallback(cb);
  addHandle(easy);
  perform();
}

EventCurler::multi_t* EventCurler::getMulti() {
  return multi_.get();
}

}} // score::curl
