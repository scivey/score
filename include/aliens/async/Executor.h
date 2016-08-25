#pragma once
#include <functional>
#include <glog/logging.h>
#include "aliens/async/VoidCallback.h"
#include "aliens/async/EventHandlerBase.h"

namespace aliens { namespace async {

class Executor {
 public:
  class EventHandler : public EventHandlerBase<Executor> {
   public:
    virtual void onStarted() {
      LOG(INFO) << "onStarted!";
    }
    virtual void onShutdownRequested() {
      getParent()->shutdown();
    }
    virtual void onBeforeShutdown() {}
    virtual void onAfterShutdown() {}
  };

  virtual EventHandler* getEventHandler() {
    return nullptr;
  }

  template<typename TCallable>
  void start(TCallable &&cb) {
    using MWrap = folly::MoveWrapper<TCallable>;
    auto wrapped = folly::makeMoveWrapper(std::move(cb));
    doStart([this, wrapped]() {
      MWrap moved = wrapped;
      TCallable cb2 = moved.move();
      auto handler = getEventHandler();
      if (handler) {
        handler->onStarted();
      }
      cb2();
    });
  }
  virtual void stop() = 0;
  virtual void submit(VoidCallback &&callback) = 0;
  virtual void doShutdown() = 0;
  template<typename TCallable>
  virtual void submit(TCallable &&item) {
    VoidCallback cb {std::move(item)};
    submit(std::move(cb));
  }

};

}} // aliens::async