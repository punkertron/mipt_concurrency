#pragma once

#include <twist/ed/std/mutex.hpp>

#include <utility>
#include <mutex>

template <typename T, class Mutex = twist::ed::std::mutex>
class Mutexed {
  class OwnerRef;

 public:
  template <typename... Args>
  explicit Mutexed(Args&&... args)
      : object_(std::forward<Args>(args)...) {
  }

  OwnerRef Acquire() & {
    return {mutex_, &object_};
  }

 private:
  T object_;
  Mutex mutex_;  // Guards access to object_

  class OwnerRef {
   public:
    OwnerRef(Mutex& mutex, T* object_ptr)
        : guard_(std::lock_guard<Mutex>(mutex)),
          object_ptr_(object_ptr) {
    }
    OwnerRef(const OwnerRef&) = delete;
    OwnerRef& operator=(const OwnerRef&) = delete;
    OwnerRef(OwnerRef&&) = delete;
    OwnerRef& operator=(OwnerRef&&) = delete;

    T* operator->() {
      return object_ptr_;
    }

    T& operator*() {
      return *object_ptr_;
    }

   private:
    std::lock_guard<Mutex> guard_;
    T* object_ptr_{nullptr};
  };
};

template <typename T>
auto Acquire(Mutexed<T>& object) {
  return object.Acquire();
}
