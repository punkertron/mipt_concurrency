#pragma once

#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/condition_variable.hpp>

#include <cstdlib>

class Semaphore {
 public:
  explicit Semaphore(size_t permits)
      : permits_(permits) {
  }

  void Acquire() {
    std::unique_lock lock(mutex_);

    while (permits_ == 0) {
      cv_.wait(lock);
    }
    --permits_;
  }

  void Release() {
    {
      std::lock_guard guard(mutex_);
      ++permits_;
    }
    cv_.notify_one();
  }

 private:
  size_t permits_ = 0;
  twist::ed::std::mutex mutex_;
  twist::ed::std::condition_variable cv_;
};
