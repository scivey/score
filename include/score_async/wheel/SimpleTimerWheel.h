#pragma once

#include <memory>
#include <vector>
#include <glog/logging.h>
#include "score_async/EvBase.h"
#include "score_async/TimerEvent.h"
#include "score_async/VoidCallback.h"
#include "score/TimerSettings.h"
#include "score_async/wheel/WheelBuffer.h"
#include "score/macros.h"

namespace score { namespace async { namespace wheel {

template<uint64_t ResolutionMS, uint64_t NSlots>
class SimpleTimerWheel {
 public:
  using base_t = score::async::EvBase;
  using timer_settings_t = score::TimerSettings;
  using timer_t = score::async::TimerEvent;
  using timer_ptr_t = std::unique_ptr<timer_t>;
  using cb_t = score::async::VoidCallback;
  static const size_t kResolution = ResolutionMS;
  static const size_t kSlots = NSlots;
  static const size_t kTimeCapacity = ResolutionMS * NSlots;

  class Entry {
   protected:
    cb_t callback_;
    uint64_t interval_ {0};
    bool repeating_ {false};
   public:
    bool good() const {
      return !!callback_;
    }
    operator bool() const {
      return good();
    }
    Entry(const cb_t& cb, uint64_t interval, bool repeating = false)
      : callback_(cb), interval_(interval), repeating_(repeating) {}

    Entry(cb_t&& cb, uint64_t interval, bool repeating = false)
      : callback_(std::forward<cb_t>(cb)), interval_(interval), repeating_(repeating) {}
    Entry(){}
    void operator()() {
      DCHECK(good());
      callback_();
    }
    uint64_t getInterval() {
      return interval_;
    }
    bool isRepeating() const {
      return repeating_;
    }
  };

  struct Slot {
    std::vector<Entry> entries;
  };

 protected:
  base_t *base_ {nullptr};
  timer_ptr_t timer_ {nullptr};
  WheelBuffer<Slot, NSlots> buffer_;

  SCORE_DISABLE_COPY_AND_ASSIGN(SimpleTimerWheel);
  SimpleTimerWheel(base_t *base): base_(base){}

  Slot& getBucket(uint64_t msecDelay) {
    uint64_t idx = msecDelay / ResolutionMS;
    if (idx > NSlots) {
      throw std::runtime_error("bucket out of range.");
    }
    return buffer_.getAt(idx);
  }
  void onTick() {
    auto& current = buffer_.getAt(0);
    for (auto& entry: current.entries) {
      Entry movedEntry = std::move(entry);
      DCHECK(!!movedEntry);
      movedEntry();
      if (movedEntry.isRepeating()) {
        addEntry(std::move(movedEntry));
      }
    }
    current.entries.clear();
    buffer_.advance();
  }
  void addEntry(Entry&& entry) {
    getBucket(entry.getInterval()).entries.push_back(std::move(entry));
  }
 public:
  static SimpleTimerWheel* createNew(base_t *base) {
    return new SimpleTimerWheel(base);
  }
  template<typename TCallable>
  void add(TCallable&& callable, uint64_t msecDelay, bool repeating) {
    std::function<void()> fn = callable;
    Entry entry {callable, msecDelay, repeating};
    addEntry(std::move(entry));
  }
  template<typename TCallable>
  void add(const TCallable& callable, uint64_t msecDelay, bool repeating) {
    std::function<void()> fn = callable;
    Entry entry {callable, msecDelay, repeating};
    addEntry(std::move(entry));
  }
  template<typename TCallable>
  void addRepeating(TCallable&& callable, uint64_t msecInterval) {
    bool repeating = true;
    add(std::forward<TCallable>(callable), msecInterval, repeating);
  }
  template<typename TCallable>
  void addOneShot(TCallable&& callable, uint64_t msecInterval) {
    bool repeating = false;
    add(std::forward<TCallable>(callable), msecInterval, repeating);
  }
  void start() {
    CHECK(!timer_);
    timer_settings_t settings {std::chrono::milliseconds {ResolutionMS}};
    timer_.reset(TimerEvent::createNewTimerEvent(base_, settings));
    timer_->setHandler([this]() {
      onTick();
    });
  }
  void stop() {
    CHECK(!!timer_);
    timer_->del();
    timer_.reset();
  }
  uint64_t endOffsetMS() const {
    return NSlots * ResolutionMS;
  }
  uint64_t resolution() const {
    return ResolutionMS;
  }
};


}}} // score::async::wheel
