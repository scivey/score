#pragma once
#include "aliens/async/ExecutorBase.h"
#include "aliens/NullablePointer.h"
#include <atomic>
namespace aliens { namespace async {

class ThreadPoolExecutor : public ExecutorBase {
 protected:
  ExecutorBase::EventHandler defaultHandler_;
  std::shared_ptr<ExecutorBase::EventHandler> nonDefaultHandler_ {nullptr};
  std::atomic<bool> handlerRetrieved_ {false};
 public:
  class Handler: public ExecutorBase::EventHandler {};
  ThreadPoolExecutor(Handler *handler): handler_(handler){}

  using HandlerOption = NullablePointer<ExecutorBase::EventHandler>;

  virtual HandlerOption getEventHandler() override {
    handlerRetrieved_.store(true);
    if (nonDefaultHandler_) {
      return HandlerOption(nonDefaultHandler_.get());
    } else {
      return HandlerOption(&defaultHandler_);
    }
  }
  virtual void setEventHandler(std::shared_ptr<EventHandler> handler) override {
    ACHECK(!handlerRetrieved_.load());
    nonDefaultHandler_ = std::move(handler);
  }
  virtual void doStop(ErrBack &&errBack) override {

  }
  virtual void doStart(ErrBack &&errBack) override {

  }
  virtual void doSubmit(VoidCallback &&work, ErrBack &&errBack) override {

  }

};

}} // aliens::async