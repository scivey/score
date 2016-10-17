#pragma once

#include <thread>
#include <functional>
#include <glog/logging.h>
#include <atomic>
#include <folly/ProducerConsumerQueue.h>
#include "score_async/VoidCallback.h"
#include "score_async/EventFDChannel.h"
#include "score_async/EvBase.h"

namespace score { namespace async {


using EBWorkQueue = folly::ProducerConsumerQueue<VoidCallback>;

class EB2Thread {
 protected:
  std::unique_ptr<std::thread> thread_ {nullptr};
  std::atomic<bool> running_ {false};
  std::unique_ptr<EvBase> base_ {nullptr};
  std::unique_ptr<EventFDChannel> workSignalChannel_ {nullptr};
  std::atomic<uintptr_t> channelPtr_ {0};
  EBWorkQueue workQueue_ {1000};

  void drainWork() {
    VoidCallback currentWork;
    while(!workQueue_.isEmpty()) {
      if (!workQueue_.read(currentWork)) {
        break;
      }
      CHECK(currentWork);
      currentWork();
    }
  }

  void createChannel() {
    workSignalChannel_ = EventFDChannel::createSmart<std::unique_ptr>(
      base_.get()
    );
    workSignalChannel_->setReadHandler([this](uint64_t x) {
      drainWork();
    });
    channelPtr_.store((uintptr_t) workSignalChannel_.get());
  }

  void makeThread() {
    CHECK(!thread_);
    thread_.reset(new std::thread([this]() {
      base_ = EvBase::createSmart<std::unique_ptr>();
      createChannel();
      while (!running_.load()) {
        this_thread::sleep_for(chrono::milliseconds(5));
      }
      TimerSettings settings {std::chrono::milliseconds (100)};
      auto loopDuration = settings.toTimeVal();
      for (;;) {
        base_->runFor(&loopDuration);
        if (!running_.load()) {
          break;
        }
      }
    }));
  }

 public:
  static EB2Thread* createNew() {
    auto instance = new EB2Thread;
    instance->makeThread();
    return instance;
  }
  template<template<class...> class TSmartPtr>
  static TSmartPtr<EB2Thread> createSmart() {
    return TSmartPtr<EB2Thread> {createNew()};
  }
  EvBase* getBase() {
    return base_.get();
  }
  void start() {
    CHECK(!running_.load());
    bool desired = true;
    bool expected = false;
    CHECK(running_.compare_exchange_strong(expected, desired));
  }
  void stop() {
    CHECK(running_.load());
    bool desired = false;
    bool expected = true;
    CHECK(running_.compare_exchange_strong(expected, desired));
  }
  void join() {
    thread_->join();
  }

  template<typename TCallable>
  void runInThread(TCallable &&callable) {
    VoidCallback cb(std::forward<TCallable>(callable));
    workQueue_.write(std::move(cb));
    auto sigChannel = channelPtr_.load();
    while (sigChannel == 0) {
      std::this_thread::sleep_for(chrono::milliseconds(5));
      sigChannel = channelPtr_.load();
    }
    auto sigPtr = (EventFDChannel*) sigChannel;
    sigPtr->send(1);
  }
};

}} // score::async
