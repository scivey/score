#pragma once

#include <memory>
#include <event2/event.h>
#include "score/util/misc.h"
#include "score/TimerSettings.h"

namespace score { namespace async {

class EvBaseCore {
 public:
  using base_t = event_base;
  using base_ptr_t = typename score::util::unique_destructor_ptr<base_t>::type;

 protected:
  base_ptr_t base_ {nullptr};
  void takeOwnershipOfBase(base_t *base) {
    base_ = score::util::asDestructorPtr<base_t>(base, event_base_free);
  }
 public:
  static EvBaseCore createWithOwnership(base_t *base) {
    EvBaseCore baseCore;
    baseCore.takeOwnershipOfBase(base);
    return baseCore;
  }
  static EvBaseCore create() {
    return createWithOwnership(event_base_new());
  }
  static EvBaseCore* newWithOwnership(base_t *base) {
    return new EvBaseCore {EvBaseCore::createWithOwnership(base)};
  }
  static EvBaseCore* createNew() {
    return newWithOwnership(event_base_new());
  }
  template<template <class...> class TSmartPtr = std::unique_ptr>
  static TSmartPtr<EvBaseCore> createSmart() {
    return TSmartPtr<EvBaseCore> {
      createNew()
    };
  }
  base_t* getBase() {
    return base_.get();
  }
  void runOnce() {
    event_base_loop(base_.get(), EVLOOP_ONCE);
  }
  void runNonBlocking() {
    event_base_loop(base_.get(), EVLOOP_NONBLOCK);
  }
  void runForever() {
    event_base_dispatch(base_.get());
  }
  void runFor(timeval *tv) {
    event_base_loopexit(base_.get(), tv);
    event_base_dispatch(base_.get());
  }
  void runFor(const TimerSettings &settings) {
    timeval tv = settings.toTimeVal();
    runFor(&tv);
  }
};

}} // score::async
