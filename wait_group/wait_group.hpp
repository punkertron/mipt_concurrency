#pragma once

#include <twist/ed/std/condition_variable.hpp>
#include <twist/ed/std/mutex.hpp>

#include <cstddef>

class WaitGroup {
 public:
  void Add(size_t count) {
    std::lock_guard lock(mutex_);
    count_ += count;
  }

  void Done() {
    std::unique_lock lock(mutex_);

    if (--count_ == 0) {
      ++generation_;
      if (waiters_ != 0) {
        cv_.notify_all();
      }
    }
  }

  void Wait() {
    std::unique_lock lock(mutex_);

    if (count_ == 0) {
      return;
    }

    const size_t generation = generation_;
    ++waiters_;

    while (generation_ == generation) {
      cv_.wait(lock);
    }

    --waiters_;
  }

 private:
  size_t count_{0};
  size_t generation_{0};
  size_t waiters_{0};
  twist::ed::std::mutex mutex_;
  twist::ed::std::condition_variable cv_;
};
