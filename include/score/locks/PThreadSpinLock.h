#pragma once

#include "score/Try.h"
#include <pthread.h>

namespace score { namespace locks {


class PThreadSpinLock {
 protected:
  pthread_spinlock_t *spinLock_ {nullptr};
  PThreadSpinLock(const PThreadSpinLock&) = delete;
  PThreadSpinLock& operator=(const PThreadSpinLock&) = delete;
  PThreadSpinLock();
  PThreadSpinLock(pthread_spinlock_t*);
 public:
  PThreadSpinLock(PThreadSpinLock&&);
  PThreadSpinLock& operator=(PThreadSpinLock&&);
  static score::Try<PThreadSpinLock> create();
  bool good() const;
  explicit operator bool() const;
  bool try_lock();
  void lock();
  void unlock();
};


}} // score::locks