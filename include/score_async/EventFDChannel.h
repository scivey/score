#pragma once

#include <memory>
#include <glog/logging.h>

#include "evs/events/EventFD.h"
#include "score_async/EvBase.h"
#include "score_async/SignalEvent.h"
#include "score_async/UnaryCallback.h"
#include "score_async/EventFDEvent.h"

namespace score { namespace async {

class EventFDChannel {
 public:
  using event_fd_t = evs::events::EventFD;
  using event_t = EventFDEvent;
  using base_t = EvBase;
  using readable_cb_t = UnaryCallback<uint64_t>;
 protected:
  EventFDChannel(){}
  base_t *base_ {nullptr};
  std::unique_ptr<event_fd_t> eventFd_ {nullptr};
  std::unique_ptr<event_t> event_ {nullptr};
  readable_cb_t readCallback_ {};
  void rebind() {
    if (event_) {
      event_->setReadHandler([this](){
        DCHECK(!!readCallback_);
        auto msg = eventFd_->read();
        msg.throwIfFailed();
        readCallback_(msg.value());
      });
    }
  }
 public:
  EventFDChannel(EventFDChannel&& other)
    : base_(other.base_),
      eventFd_(std::move(other.eventFd_)),
      event_(std::move(other.event_)),
      readCallback_(std::move(other.readCallback_)) {
      rebind();
  }
  EventFDChannel& operator=(EventFDChannel&& other) {
    std::swap(base_, other.base_);
    std::swap(eventFd_, other.eventFd_);
    std::swap(event_, other.event_);
    std::swap(readCallback_, other.readCallback_);
    rebind();
    other.rebind();
    return *this;
  }

  template<typename TCallable>
  void setReadHandler(TCallable&& callable) {
    readCallback_ = std::forward<TCallable>(callable);
  }

  static EventFDChannel* createNew(base_t *base) {
    auto instance = new EventFDChannel;
    instance->base_ = base;
    instance->eventFd_.reset(
      new event_fd_t{event_fd_t::create()}
    );
    int fdNo = instance->eventFd_->getFd().value();
    instance->event_.reset(EventFDEvent::createNewEvent(
      base, fdNo, EV_READ | EV_PERSIST
    ));
    instance->event_->add();
    instance->rebind();
    return instance;
  }

  template<template<class...> class TSmartPtr = std::unique_ptr>
  static TSmartPtr<EventFDChannel> createSmart(base_t *base) {
    return TSmartPtr<EventFDChannel> {
      createNew(base)
    };
  }

  void send(uint64_t msg) {
    eventFd_->write(msg).throwIfFailed();
  }
};

}} // score::async
