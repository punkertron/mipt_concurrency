#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/futex.hpp>

#include <cstdint>

class CondVar {
 public:
  // Mutex - BasicLockable
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable
  template <class Mutex>
  void Wait(Mutex& mutex) {
    const uint32_t epoch = epoch_.load();
    waiters_.fetch_add(1);

    mutex.unlock();
    twist::ed::futex::Wait(epoch_, epoch);
    waiters_.fetch_sub(1);
    mutex.lock();
  }

  void NotifyOne() {
    if (waiters_.load() != 0) {
      const auto wake_key = twist::ed::futex::PrepareWake(epoch_);
      epoch_.fetch_add(1);
      twist::ed::futex::WakeOne(wake_key);
    }
  }

  void NotifyAll() {
    if (waiters_.load() != 0) {
      const auto wake_key = twist::ed::futex::PrepareWake(epoch_);
      epoch_.fetch_add(1);
      twist::ed::futex::WakeAll(wake_key);
    }
  }

 private:
  twist::ed::std::atomic<uint32_t> epoch_{0};
  twist::ed::std::atomic<uint32_t> waiters_{0};
};
