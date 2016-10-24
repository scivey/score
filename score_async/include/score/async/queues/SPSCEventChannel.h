#pragma once
#include <mutex>
#include "score/exceptions/ScoreError.h"
#include "score_async/EventFD.h"
#include "score_async/queues/SPSCQueue.h"
#include "score_async/queues/QueueError.h"

namespace score { namespace async { namespace queues {


template<typename T>
class SPSCEventChannel {
 public:
  using queue_t = SPSCQueue<T>;
  using queue_ptr_t = std::unique_ptr<queue_t>;
 protected:
  EventFD eventFD_;
  queue_ptr_t queue_ {nullptr};

 public:
  SPSCEventChannel(EventFD&& efd, queue_ptr_t&& spQueue)
    : eventFD_(std::move(efd)), queue_(std::move(spQueue)) {}

  bool good() const {
    return !!eventFD_ && !!queue_;
  }
  explicit operator bool() const {
    return good();
  }
  static SPSCEventChannel create() {
    return SPSCEventChannel(
      EventFD::create().value(),
      util::makeUnique<queue_t>(size_t {10000})
    );
  }

  static SPSCEventChannel* createNew() {
    return new SPSCEventChannel{create()};
  }

  score::Try<Unit> trySend(T&& msg) {
    DCHECK(good());
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