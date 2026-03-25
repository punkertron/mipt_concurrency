#pragma once

#include "error.hpp"
#include "shared_state.hpp"

#include <memory>
#include <utility>

template <typename T>
class Future {
  template <typename U>
  friend class Promise;

 public:
  // Non-copyable
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  // Movable
  Future(Future&&) = default;
  Future& operator=(Future&&) = default;

  // One-shot
  T Get() {
    if (!shared_state_) {
      throw NoStateError();
    }

    std::unique_lock locker{shared_state_->mutex_};

    if (!shared_state_->future_valid_) {
      throw NoStateError();
    }

    while (!shared_state_->result_ready_) {
      shared_state_->result_ready_cv_.wait(locker);
    }
    shared_state_->future_valid_ = false;

    if (!shared_state_->result_.has_value()) {
      std::rethrow_exception(shared_state_->result_.error());
    }

    return std::move(shared_state_->result_.value());
  }

  bool Valid() const {
    if (!shared_state_) {
      return false;
    }

    std::lock_guard guard{shared_state_->mutex_};
    return shared_state_->future_valid_;
  }

 private:
  explicit Future(std::shared_ptr<SharedState<T>> shared_state)
      : shared_state_(shared_state) {
  }

 private:
  std::shared_ptr<SharedState<T>> shared_state_;
};
