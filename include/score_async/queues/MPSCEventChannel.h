#pragma once
#include <mutex>
#include "score/locks/SpinLock.h"
#include "score/exceptions/ScoreError.h"
#include "score_async/EventFD.h"
#include "score_async/queues/SPSCQueue.h"
#include "score_async/queues/QueueError.h"

namespace score { namespace async { namespace queues {


template<typename T>
class MPSCEventChannel {
 public:
  using queue_t = SPSCQueue<T>;
  using queue_ptr_t = std::unique_ptr<queue_t>;
  using lock_t = score::locks::SpinLock;
 protected:
  EventFD eventFD_;
  queue_ptr_t queue_ {nullptr};
  lock_t lock_;

 public:
  MPSCEventChannel(EventFD&& efd, queue_ptr_t&& spQueue, lock_t&& lock)
    : eventFD_(std::move(efd)), queue_(std::move(spQueue)), lock_(std::move(lock)){}

  bool good() const {
    return !!eventFD_ && !!queue_ && !!lock_;
  }
  explicit operator bool() const {
    return good();
  }
  static MPSCEventChannel create() {
    return MPSCEventChannel(
      EventFD::create().value(),
      util::makeUnique<queue_t>(size_t {20000}),
      lock_t::create().value()
    );
  }

  static MPSCEventChannel* createNew() {
    return new MPSCEventChannel{create()};
  }

  score::Try<Unit> tryLockAndSend(T&& msg) {
    DCHECK(good());
    std::lock_guard<lock_t> guard {lock_};
    if (queue_->try_enqueue(std::forward<T>(msg))) {
      auto writeRes = eventFD_.write(1);
      if (writeRes.hasException()) {
        return util::makeTryFailure<Unit, PartialWriteError>(
          "Enqueued message, but could not signal EventFD."
        );
      }
      return util::makeTrySuccess<Unit>();
    }
    return util::makeTryFailure<Unit, QueueFull>(
      "Queue is full."
    );
  }

  score::Try<bool> tryRead(T& result) {
    DCHECK(good());
    auto readRes = eventFD_.read();
    if (readRes.hasException() && !readRes.template hasException<EventFD::NotReady>()) {
      return score::Try<bool> {
        std::move(readRes.exception())
      };
    }
    return util::makeTrySuccess<bool>(
      queue_->try_dequeue(result)
    );
  }

  score::Try<int> getFDNum() {
    return eventFD_.getFDNum();
  }
};

}}} // score::async::queues