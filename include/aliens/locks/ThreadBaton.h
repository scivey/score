#pragma once
#include <exception>
#include <stdexcept>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace aliens { namespace locks {

class ThreadBaton {
 protected:
  std::atomic<bool> posted_ {false};
  std::mutex mutex_;
  std::condition_variable condition_;
  ThreadBaton(const ThreadBaton&) = delete;
  ThreadBaton& operator=(const ThreadBaton&) = delete;

 public:
  ThreadBaton(){}
  ThreadBaton(ThreadBaton&&) = default;
  ThreadBaton& operator=(ThreadBaton&&) = default;
  class AlreadyPosted : public std::runtime_error {
   public:
    AlreadyPosted();
  };
  void post();
  void wait();
  bool isDone() const;

 protected:
  void doPost();
};

}} // aliens::locks
