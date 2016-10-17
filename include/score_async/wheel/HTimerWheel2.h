#pragma once

#include <memory>
#include <vector>
#include <glog/logging.h>
#include "evs/events2/EvBase.h"
#include "evs/events2/TimerEvent.h"
#include "evs/events2/VoidCallback.h"
#include "evs/events/TimerSettings.h"
#include "evs/events2/wheel/WheelBuffer.h"
#include "evs/events2/wheel/SimpleTimerWheel.h"
#include "evs/macros.h"
#include "evs/logging.h"

namespace evs { namespace events2 { namespace wheel {

#define HH_BUCKET1_RES 5
#define HH_BUCKET1_SLOTS 5
#define HH_BUCKET2_RES 25
#define HH_BUCKET2_SLOTS 4

#define HH_BUCKET3_RES 100
#define HH_BUCKET3_SLOTS 5

#define HH_BUCKET4_RES 500
#define HH_BUCKET4_SLOTS 4

#define HH_TICK1_COUNTER 5
#define HH_TICK2_COUNTER 4
#define HH_TICK3_COUNTER 5

class HTimerWheel2 {
 public:
  using base_t = evs::events2::EvBase;
  using timer_settings_t = evs::events::TimerSettings;
  using timer_t = evs::events2::TimerEvent;
  using timer_ptr_t = std::unique_ptr<timer_t>;
  using cb_t = evs::events2::VoidCallback;
  using clock_t = std::chrono::steady_clock;
  using time_delta_t = std::chrono::milliseconds;
  using time_point_t = time_delta_t;

  class Entry {
   protected:
    cb_t callback_;
    time_delta_t interval_ {0};
    time_point_t addedTime_;
    bool repeating_ {false};
   public:
    bool good() const {
      return !!callback_;
    }
    operator bool() const {
      return good();
    }
    Entry(const cb_t& cb, time_delta_t interval, bool repeating = false)
      : callback_(cb), interval_(interval), repeating_(repeating) {}

    void setAddedTime(time_point_t added) {
      // LOG(INFO) << "setAddedTime: " << added.count();
      addedTime_ = added;
    }

    time_delta_t getRemainingTime(time_point_t nowTime) {
      auto endTime = addedTime_ + interval_;
      if (endTime <= nowTime) {
        return time_delta_t {0};
      }
      return endTime - nowTime;
    }

    Entry(cb_t&& cb, time_delta_t interval, bool repeating = false)
      : callback_(std::forward<cb_t>(cb)), interval_(interval), repeating_(repeating) {}
    Entry(){}
    void operator()() {
      DCHECK(good());
      callback_();
    }
    time_delta_t getInterval() {
      return interval_;
    }
    bool isRepeating() const {
      return repeating_;
    }
  };

  struct Slot {
    std::vector<Entry> entries;
  };

  template<size_t ResolutionMS, size_t NSlots>
  class Bucket {
   protected:
    WheelBuffer<Slot, NSlots> buffer_;
    bool isBottom_ {false};
   public:
    Bucket(){}
    Bucket(bool isBottom): isBottom_(isBottom){}
    time_delta_t timeCapacity() const {
      return time_delta_t {ResolutionMS * NSlots};
    }
    template<typename ...Types>
    void say(const char *fmt, Types&&... args) {
      auto subMessage = folly::sformat(fmt, std::forward<Types>(args)...);
      EVS_INFO("Bucket[{}/{}]: '{}'", ResolutionMS, NSlots, subMessage);
    }
    Slot& getSlotAt(size_t idx) {
      CHECK(idx < NSlots);
      return buffer_.getAt(idx);
    }
    size_t getSlotIdx(time_delta_t msecDelay) {
      if (msecDelay > timeCapacity()) {
        throw std::runtime_error("slot out of range.");
      }
      size_t totalMsec = msecDelay.count();
      size_t overflow = totalMsec % ResolutionMS;
      size_t toConsider = totalMsec - overflow;
      if (totalMsec > 0) {
        ;
        // say("toConsider: {}/{}", toConsider, totalMsec);
      }
      size_t idx = 0;
      size_t current = 0;
      if (totalMsec < ResolutionMS) {
        idx = 0;
      } else {
        for (;;) {
          current += ResolutionMS;
          if (overflow > 0) {
            if (current < toConsider) {
              idx++;
            } else {
              break;
            }
          } else {
            if (current <= toConsider) {
              idx++;
            } else {
              break;
            }
          }
        }
      }
      if (isBottom_) {
        if (idx == 0) {
          size_t zeroDelta = overflow;
          size_t tickDelta = ResolutionMS - overflow;
          if (tickDelta < zeroDelta) {
            idx++;
          }
        }
      } else if (totalMsec < ResolutionMS) {
        idx = 0;
      }

      if (totalMsec > 0) {
        ;
        // say("result idx: {}", idx);
      }
      return idx;
    }
    Slot& getSlot(time_delta_t delay) {
      return getSlotAt(getSlotIdx(delay));
    }
    void addEntry(time_point_t nowTime, Entry&& entry) {
      auto remaining = entry.getRemainingTime(nowTime);
      auto slotIdx = getSlotIdx(remaining);
      // say("adding entry with {}ms remaining to slot #{}", remaining.count(), slotIdx);
      getSlotAt(slotIdx).entries.push_back(std::move(entry));
    }
    void advance() {
      buffer_.advance();
    }
  };

 protected:
  base_t *base_ {nullptr};
  Bucket<HH_BUCKET1_RES, HH_BUCKET1_SLOTS> bucket1_ {true};
  timer_ptr_t timer1_ {nullptr};
  Bucket<HH_BUCKET2_RES, HH_BUCKET2_SLOTS> bucket2_;
  timer_ptr_t timer2_ {nullptr};
  Bucket<HH_BUCKET3_RES, HH_BUCKET3_SLOTS> bucket3_;
  timer_ptr_t timer3_ {nullptr};
  Bucket<HH_BUCKET4_RES, HH_BUCKET4_SLOTS> bucket4_;
  timer_ptr_t timer4_ {nullptr};
  time_point_t cachedNow_;
  bool nowInititialized_ {false};

  void updateNowOffset() {
    auto steadyNow = clock_t::now().time_since_epoch();
    cachedNow_ = std::chrono::duration_cast<time_point_t>(steadyNow);
    nowInititialized_ = true;
  }

  time_point_t getNowOffset() {
    if (!nowInititialized_) {
      updateNowOffset();
    }
    return cachedNow_;
  }

  HTimerWheel2(base_t *base): base_(base){}

  EVS_DISABLE_COPY_AND_ASSIGN(HTimerWheel2);

  void addEntry(Entry&& entry) {
    auto now = getNowOffset();
    auto offset = entry.getRemainingTime(now);
    if (offset < bucket1_.timeCapacity()) {
      EVS_INFO("entry has offset {}; adding to bucket1.", offset.count());
      bucket1_.addEntry(now, std::move(entry));
    } else if (offset < bucket2_.timeCapacity()) {
      EVS_INFO("entry has offset {}; adding to bucket2.", offset.count());
      bucket2_.addEntry(now, std::move(entry));
    } else if (offset < bucket3_.timeCapacity()) {
      EVS_INFO("entry has offset {}; adding to bucket3.", offset.count());
      bucket3_.addEntry(now, std::move(entry));
    } else if (offset < bucket4_.timeCapacity()) {
      EVS_INFO("entry has offset {}; adding to bucket4.", offset.count());
      bucket4_.addEntry(now, std::move(entry));
    } else {
      throw std::runtime_error(folly::sformat(
        "entry offset out of range: {}", offset.count()
      ));
    }
  }

  std::vector<Entry> repeatingEntriesToAdd_;
  std::vector<Entry> subEntriesToAdd_;
  std::vector<Entry> entriesToExecute_;


  void executePendingEntries() {
    for (auto& entry: entriesToExecute_) {
      Entry movedEntry = std::move(entry);
      CHECK(!!movedEntry);
      movedEntry();
      if (movedEntry.isRepeating()) {
        repeatingEntriesToAdd_.push_back(std::move(movedEntry));
      }
    }
    entriesToExecute_.clear();
  }

  void reAddRepeatingEntries() {
    for (auto &entry: repeatingEntriesToAdd_) {
      addNewEntry(std::move(entry));
    }
    repeatingEntriesToAdd_.clear();
  }

  void addPendingExecution(Entry&& entry) {
    CHECK(!!entry);
    entriesToExecute_.push_back(std::move(entry));
  }

  void addSubEntries() {
    for (auto& entry: subEntriesToAdd_) {
      addEntry(std::move(entry));
    }
    subEntriesToAdd_.clear();
  }


  using entry_vec_t = std::vector<Entry>;
  void processUpperBucketSlots(entry_vec_t& entries) {
    for (auto& entry: entries) {
      auto remaining = entry.getRemainingTime(getNowOffset());
      auto remainingCount = remaining.count();
      size_t bucket1Res = HH_BUCKET1_RES;
      if (remainingCount < bucket1Res) {
        size_t zeroDelta = remainingCount;
        size_t resDelta = bucket1Res - remainingCount;
        if (zeroDelta < resDelta) {
          addPendingExecution(std::move(entry));
        }
      }
      if (entry) {
        subEntriesToAdd_.push_back(std::move(entry));
      }
    }
  }


  void processBucket1() {
    auto& current = bucket1_.getSlot(time_delta_t {0});
    for (auto& entry: current.entries) {
      if (entry) {
        addPendingExecution(std::move(entry));
      }
    }
    current.entries.clear();
  }

  void processBucket2() {
    auto& current = bucket2_.getSlot(time_delta_t {0});
    processUpperBucketSlots(current.entries);
    current.entries.clear();
  }

  void processBucket3() {
    auto& current = bucket3_.getSlot(time_delta_t {0});
    processUpperBucketSlots(current.entries);
    current.entries.clear();
  }

  void processBucket4() {
    auto& current = bucket4_.getSlot(time_delta_t {0});
    processUpperBucketSlots(current.entries);
    current.entries.clear();
  }


  void onTick1Impl() {
    updateNowOffset();
    processBucket1();
    executePendingEntries();
  }
  void onTick2Impl() {
    processBucket2();
    executePendingEntries();
    onTick1Impl();
  }
  void onTick3Impl() {
    processBucket3();
    executePendingEntries();
    onTick2Impl();
  }
  void onTick4Impl() {
    processBucket4();
    executePendingEntries();
    onTick3Impl();
  }
  void onAfterTick() {
    executePendingEntries();
    updateNowOffset();
    addSubEntries();
    reAddRepeatingEntries();
    executePendingEntries();
  }
  size_t tick1Counter_ {0};

  void onTick1() {
    tick1Counter_++;
    if (tick1Counter_ == HH_TICK1_COUNTER) {
      tick1Counter_ = 0;
      return;
    }
    // LOG(INFO) << "onTick1";
    onTick1Impl();
    bucket1_.advance();
    onAfterTick();
  }

  size_t tick2Counter_ {0};
  void onTick2() {
    tick2Counter_++;
    if (tick2Counter_ == HH_TICK2_COUNTER) {
      tick2Counter_ = 0;
      return;
    }
    // LOG(INFO) << "onTick2";
    onTick2Impl();
    bucket1_.advance();
    bucket2_.advance();
    onAfterTick();
  }

  size_t tick3Counter_ {0};
  void onTick3() {
    tick3Counter_++;
    // LOG(INFO) << "onTick3";
    if (tick3Counter_ == HH_TICK3_COUNTER) {
      tick3Counter_ = 0;
      return;
    }
    onTick3Impl();
    bucket1_.advance();
    bucket2_.advance();
    bucket3_.advance();
    onAfterTick();
  }

  void onTick4() {
    // LOG(INFO) << "onTick3";
    onTick4Impl();
    bucket1_.advance();
    bucket2_.advance();
    bucket3_.advance();
    bucket4_.advance();
    onAfterTick();
  }

  void addNewEntry(Entry&& entry) {
    entry.setAddedTime(getNowOffset());
    addEntry(std::move(entry));
  }

 public:
  static HTimerWheel2* createNew(base_t *base) {
    return new HTimerWheel2(base);
  }

  template<typename TCallable>
  void add(TCallable&& callable, time_delta_t msecDelay, bool repeating) {
    addNewEntry(Entry {callable, msecDelay, repeating});
  }

  template<typename TCallable>
  void add(const TCallable& callable, time_delta_t msecDelay, bool repeating) {
    addNewEntry(Entry {callable, msecDelay, repeating});
  }

  template<typename TCallable>
  void addRepeating(TCallable&& callable, time_delta_t msecInterval) {
    bool repeating = true;
    add(std::forward<TCallable>(callable), msecInterval, repeating);
  }

  template<typename TCallable>
  void addOneShot(TCallable&& callable, time_delta_t msecInterval) {
    bool repeating = false;
    add(std::forward<TCallable>(callable), msecInterval, repeating);
  }

  void start() {
    updateNowOffset();
    CHECK(!timer1_ && !timer2_ && !timer3_ && !timer4_);
    timer_settings_t settings1 {std::chrono::milliseconds {HH_BUCKET1_RES}};
    timer1_.reset(TimerEvent::createNewTimerEvent(base_, settings1));
    timer1_->setHandler([this]() { onTick1(); });
    timer_settings_t settings2 {std::chrono::milliseconds {HH_BUCKET2_RES}};
    timer2_.reset(TimerEvent::createNewTimerEvent(base_, settings2));
    timer2_->setHandler([this]() { onTick2(); });
    timer_settings_t settings3 {std::chrono::milliseconds {HH_BUCKET3_RES}};
    timer3_.reset(TimerEvent::createNewTimerEvent(base_, settings3));
    timer3_->setHandler([this]() { onTick3(); });
    timer_settings_t settings4 {std::chrono::milliseconds {HH_BUCKET4_RES}};
    timer4_.reset(TimerEvent::createNewTimerEvent(base_, settings4));
    timer4_->setHandler([this]() { onTick4(); });
  }
  void stop() {
    CHECK(!!timer1_ && !!timer2_ && !!timer3_ && !!timer4_);
    timer1_->del();
    timer1_.reset();
    timer2_->del();
    timer2_.reset();
    timer3_->del();
    timer3_.reset();
    timer4_->del();
    timer4_.reset();
  }

  auto getNow() -> decltype(getNowOffset()) {
    return getNowOffset();
  }
};

}}} // evs::events2::wheel
