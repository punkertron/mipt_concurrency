#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/futex.hpp>
#include <twist/ed/wait/spin.hpp>

#include <cstdint>

class Mutex {
 public:
  void Lock() {
    uint32_t expected = 0;
    if (state_.compare_exchange_strong(expected, 1)) {
      return;
    }

    twist::ed::SpinWait spin;
    for (;;) {
      const auto val = state_.load();

      if (val == 0) {
        expected = 0;
        if (state_.compare_exchange_strong(expected, 2)) {
          return;
        }
        continue;
      }

      if (!spin.ConsiderParking()) {
        spin();
        continue;
      }

      if (val == 1) {
        expected = 1;
        state_.compare_exchange_strong(expected, 2);
      }

      twist::ed::futex::Wait(state_, 2);
    }
  }

  void Unlock() {
    const auto wake_key = twist::ed::futex::PrepareWake(state_);
    if (const auto old = state_.exchange(0); old == 2) {
      twist::ed::futex::WakeOne(wake_key);
    }
  }

  // BasicLockable
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable

  void lock() {  // NOLINT
    Lock();
  }

  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  // 0 - unlocked
  // 1 - locked
  // 2 - locked, possible with waiters
  twist::ed::std::atomic<uint32_t> state_{0};
};
