#pragma once

#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/condition_variable.hpp>

#include <cstddef>

class CyclicBarrier {
 public:
  explicit CyclicBarrier(size_t participants)
      : total_participants_(participants) {
  }

  void ArriveAndWait() {
    std::unique_lock lock(mutex_);
    if (participants_++ == total_participants_ - 1) {
      participants_ = 0;
      ++current_epoch_;
      lock.unlock();
      cv_.notify_all();
    } else {
      const auto epoch = current_epoch_;
      while (current_epoch_ == epoch) {
        cv_.wait(lock);
      }
    }
  }

 private:
  const size_t total_participants_ = 0;
  size_t participants_ = 0;
  size_t current_epoch_ = 0;
  twist::ed::std::mutex mutex_;
  twist::ed::std::condition_variable cv_;
};
