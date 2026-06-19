#pragma once

#include <new>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/spin.hpp>

/*
 * Scalable Queue SpinLock
 *
 * Usage:
 *
 * QueueSpinLock spinlock;
 *
 * {
 *   QueueSpinLock::Guard guard{spinlock};  // <-- Acquire spinlock
 *   // <-- Critical section
 * }  // <-- Release spinlock (~Guard)
 *
 */

class QueueSpinLock {
 public:
  class alignas(std::hardware_destructive_interference_size) Guard {
    friend class QueueSpinLock;

   public:
    explicit Guard(QueueSpinLock& host)
        : host_(host) {
      host_.Acquire(this);
    }

    // Non-copyable
    Guard(const Guard&) = delete;
    Guard& operator=(const Guard&) = delete;

    // Non-movable
    Guard(Guard&&) = delete;
    Guard& operator=(Guard&&) = delete;

    ~Guard() {
      host_.Release(this);
    }

   private:
    QueueSpinLock& host_;
    twist::ed::std::atomic<Guard*> next_{nullptr};
    twist::ed::std::atomic<bool> is_owner_{false};
  };

 private:
  void Acquire(Guard* waiter) {
    auto* prev_tail = tail_.exchange(waiter, std::memory_order_acq_rel);
    if (prev_tail == nullptr) {
      // lock acquired immediately
      return;
    }

    prev_tail->next_.store(waiter, std::memory_order_release);
    twist::ed::SpinWait spin_wait;
    while (!waiter->is_owner_.load(std::memory_order_acquire)) {
      spin_wait();
    }
  }

  void Release(Guard* owner) {
    auto* next = owner->next_.load(std::memory_order_acquire);
    if (next == nullptr) {
      if (auto expected = owner; tail_.compare_exchange_strong(
              expected, nullptr, std::memory_order_release,
              std::memory_order_relaxed)) {
        return;
      }

      twist::ed::SpinWait spin_wait;
      while ((next = owner->next_.load(std::memory_order_acquire)) == nullptr) {
        spin_wait();
      }
    }

    next->is_owner_.store(true, std::memory_order_release);
  }

 private:
  twist::ed::std::atomic<Guard*> tail_{nullptr};
};
