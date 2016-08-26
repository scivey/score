#include <glog/logging.h>
#include "aliens/locks/ThreadBaton.h"
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>



using namespace std;

namespace aliens { namespace locks {

ThreadBaton::AlreadyPosted::AlreadyPosted()
  : std::runtime_error("AlreadyPosted") {}


void ThreadBaton::post() {
  LOG(INFO) << "start post().";
  std::unique_lock<std::mutex> lk{mutex_};
  doPost();
  lk.unlock();
  condition_.notify_one();
  LOG(INFO) << "end post().";
}

void ThreadBaton::wait() {
  LOG(INFO) << "start wait().";
  if (posted_.load()) {
    // fast path
    return;
  }
  std::unique_lock<std::mutex> lk{mutex_};

  // check if producer posted between `.load()` and lock acquisition.
  // otherwise consumer could get stuck (race condition)
  if (posted_.load()) {
    return;
  }

  condition_.wait(lk, [this]() {
    return posted_.load();
  });

  for (;;) {
    if (posted_.load()) {
      break;
    }
    this_thread::sleep_for(chrono::milliseconds(200));
  }
  LOG(INFO) << "end wait()";
}

bool ThreadBaton::isDone() const {
  return posted_.load();
}

// NB lock must be held (protected method)
void ThreadBaton::doPost() {
  if (posted_.load()) {
    throw AlreadyPosted();
  }
  bool expected = false;
  bool desired = true;
  if (!posted_.compare_exchange_strong(expected, desired)) {
    throw AlreadyPosted();
  }
}

}} // aliens::locks
