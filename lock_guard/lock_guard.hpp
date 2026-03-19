#pragma once

#include <mutex>

template <typename Mutex>
class LockGuard {
 public:
  explicit LockGuard(Mutex& mutex)
      : m_(mutex) {
    m_.lock();
  }

  LockGuard(Mutex& mutex, std::adopt_lock_t)
      : m_(mutex) {
  }

  // Non-copyable
  LockGuard(const LockGuard&) = delete;
  LockGuard& operator=(const LockGuard&) = delete;

  // Non-movable
  LockGuard(LockGuard&&) = delete;
  LockGuard& operator=(LockGuard&&) = delete;

  ~LockGuard() {
    m_.unlock();
  }

 private:
  Mutex& m_;
};
