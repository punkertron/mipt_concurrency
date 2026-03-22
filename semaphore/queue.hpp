#pragma once

#include "tagged_semaphore.hpp"

#include <deque>

// Bounded Blocking Multi-Producer/Multi-Consumer (MPMC) Queue

template <typename T>
class BoundedBlockingQueue {
 public:
  explicit BoundedBlockingQueue(size_t capacity)
      : queue_lock_(1),
        free_slots_(capacity),
        available_items_(0) {
  }

  void Put(T val) {
    auto slot = free_slots_.Acquire();
    auto lock = queue_lock_.Acquire();
    queue_.push_back(std::move(val));
    queue_lock_.Release(std::move(lock));
    available_items_.Release(std::move(slot));
  }

  T Take() {
    auto item = available_items_.Acquire();
    auto lock = queue_lock_.Acquire();
    auto val = std::move(queue_.front());
    queue_.pop_front();
    queue_lock_.Release(std::move(lock));
    free_slots_.Release(std::move(item));
    return val;
  }

 private:
  // Tags
  struct QueueLockTag {};
  struct QueueTag {};

 private:
  TaggedSemaphore<QueueLockTag> queue_lock_;  // for queue_ access
  TaggedSemaphore<QueueTag> free_slots_;
  TaggedSemaphore<QueueTag> available_items_;
  std::deque<T> queue_;
};
