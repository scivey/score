#pragma once
#include <functional>
#include <glog/logging.h>
#include "aliens/async/VoidCallback.h"
#include "aliens/async/ErrBack.h"
#include "aliens/MoveWrapper.h"
#include "aliens/async/EventHandlerBase.h"
#include "aliens/NullablePointer.h"

namespace aliens { namespace async {

class Executor {
 public:
  virtual void stop(ErrBack&&) = 0;
  virtual void start(ErrBack&&) = 0;
  virtual void submit(VoidCallback&&, ErrBack&&) = 0;
  virtual ~Executor() = default;
};


class ExecutorBase : public Executor {
 public:
  class EventHandler : public EventHandlerBase<ExecutorBase>,
                       public std::enable_shared_from_this<EventHandler> {
   public:
    virtual void onBeforeStart(ErrBack &&cb) {
      LOG(INFO) << "onBeforeStart!";
      cb();
    }
    virtual void onAfterStart(ErrBack &&cb) {
      LOG(INFO) << "onAfterStart!";
      cb();
    }
    virtual void onBeforeStop(ErrBack &&cb) {
      LOG(INFO) << "onBeforeStop!";
      cb();
    }
    virtual void onAfterStop(ErrBack &&cb) {
      LOG(INFO) << "onAfterStop!";
      cb();
    }
    virtual void onStartRequested(ErrBack &&cb) {
      auto self = shared_from_this();
      using MWrap = MoveWrapper<ErrBack>;
      auto wrapped = makeMoveWrapper(cb);
      onBeforeStart([self, this, wrapped](const ErrBack::except_option &ex1) {
        if (ex1) {
            MWrap unwrapped = wrapped;
            ErrBack movedCb = unwrapped.move();
            movedCb(ex1);
        } else {
          getParent()->doStart([self, this, wrapped](const ErrBack::except_option &ex2) {
            if (ex2) {
              MWrap unwrapped = wrapped;
              ErrBack movedCb = unwrapped.move();
              movedCb(ex2);
            } else {
              onAfterStart([self, this, wrapped](const ErrBack::except_option &ex3) {
                MWrap unwrapped = wrapped;
                ErrBack movedCb = unwrapped.move();
                movedCb(ex3);
              });
            }
          });
        }
      });
    }
    virtual void onStopRequested(ErrBack &&cb) {
      auto self = shared_from_this();
      using MWrap = MoveWrapper<ErrBack>;
      auto wrapped = makeMoveWrapper(cb);
      onBeforeStop([self, this, wrapped](const ErrBack::except_option &ex1) {
        if (ex1) {
          MWrap unwrapped = wrapped;
          ErrBack movedCb = unwrapped.move();
          movedCb(ex1);
        } else {
          getParent()->doStop([self, this, wrapped](const ErrBack::except_option &ex2) {
            if (ex2) {
              MWrap unwrapped = wrapped;
              ErrBack movedCb = unwrapped.move();
              movedCb(ex2);
            } else {
              onAfterStop([self, this, wrapped](const ErrBack::except_option &ex3) {
                MWrap unwrapped = wrapped;
                ErrBack movedCb = unwrapped.move();
                movedCb(ex3);
              });
            }
          });
        }
      });
    }
    virtual void onSubmitRequested(VoidCallback &&work, ErrBack &&cb) {
      auto self = shared_from_this();
      using WorkWrap = MoveWrapper<VoidCallback>;
      using ErrWrap = MoveWrapper<ErrBack>;
      auto wrappedWork = makeMoveWrapper(work);
      auto wrappedCb = makeMoveWrapper(cb);
      getParent()->doSubmit(
        [self, this, wrappedWork]() {
          WorkWrap unwrapped = wrappedWork;
          VoidCallback movedWork = unwrapped.move();
          movedWork();
        },
        [self, this, wrappedCb](const ErrBack::except_option &ex) {
          if (ex) {
            onSubmitError(ex.value());
          } else {
            onSubmitSuccess();
          }
          ErrWrap unwrapped = wrappedCb;
          ErrBack movedCb = unwrapped.move();
          movedCb(ex);
        }
      );
    }
    virtual void onSubmitSuccess() {
      LOG(INFO) << "onSubmitSuccess";
    }
    virtual void onSubmitError(const std::exception &ex) {
      LOG(INFO) << "onSubmitError!! : " << ex.what();
    }
  };


  void stop(ErrBack &&cb) override {
    auto handler = getEventHandler();
    if (handler) {
      handler->onStopRequested(std::move(cb));
    } else {
      doStop(std::move(cb));
    }
  }

  void start(ErrBack &&cb) override {
    auto handler = getEventHandler();
    if (handler) {
      handler->onStartRequested(std::move(cb));
    } else {
      doStart(std::move(cb));
    }
  }

  void submit(VoidCallback &&work, ErrBack &&cb) override {
    auto handler = getEventHandler();
    if (handler) {
      handler->onSubmitRequested(std::move(work), std::move(cb));
    } else {
      doSubmit(std::move(work), std::move(cb));
    }
  }

  virtual NullablePointer<EventHandler> getEventHandler() = 0;
  virtual void setEventHandler(std::shared_ptr<EventHandler>) = 0;
  virtual void doStop(ErrBack&&) = 0;
  virtual void doStart(ErrBack&&) = 0;
  virtual void doSubmit(VoidCallback&& work, ErrBack &&cb) = 0;
};

}} // aliens::async