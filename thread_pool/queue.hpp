#pragma once

#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/condition_variable.hpp>
#include <queue>

#include <optional>

// Unbounded blocking multi-producers/multi-consumers (MPMC) queue

template <typename T>
class UnboundedBlockingQueue {
 public:
  void Push(T val) {
    std::lock_guard guard(mutex_);
    queue_.push(std::move(val));
    ready_to_pop_cv_.notify_one();
  }

  std::optional<T> Pop() {
    std::unique_lock locker(mutex_);
    while (queue_.empty() && !closed_) {
      ready_to_pop_cv_.wait(locker);
    }
    if (queue_.empty() && closed_) {
      return std::nullopt;
    }
    T val = std::move(queue_.front());
    queue_.pop();
    return val;
  }

  void Close() {
    std::lock_guard guard(mutex_);
    closed_ = true;
    ready_to_pop_cv_.notify_all();
  }

 private:
  std::queue<T> queue_;
  twist::ed::std::mutex mutex_;
  twist::ed::std::condition_variable ready_to_pop_cv_;
  bool closed_{false};
};
