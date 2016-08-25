#pragma once


template<typename T>
class Synchronized {
 public:
  std::mutex mutex_;
};