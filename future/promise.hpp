#pragma once

#include "error.hpp"
#include "future.hpp"
#include "shared_state.hpp"

#include <memory>
#include <utility>

template <typename T>
class Promise {
 public:
  Promise()
      : shared_state_(std::make_shared<SharedState<T>>()) {
  }

  // Non-copyable
  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;

  // Movable
  Promise(Promise&&) = default;
  Promise& operator=(Promise&& that) = default;

  ~Promise() {
    if (!shared_state_) {
      return;
    }

    std::lock_guard guard{shared_state_->mutex_};

    if (!shared_state_->result_ready_) {
      shared_state_->result_ = typename SharedState<T>::Result(
          std::unexpected(std::make_exception_ptr(BrokenPromiseError())));
      shared_state_->result_ready_ = true;
      shared_state_->result_ready_cv_.notify_one();
    }
  }

  // One-shot
  Future<T> MakeFuture() {
    if (!shared_state_) {
      throw NoStateError();
    }

    std::lock_guard guard{shared_state_->mutex_};

    if (shared_state_->future_retrieved_) {
      throw PromiseAlreadySatisfiedError();
    }

    shared_state_->future_retrieved_ = true;
    return Future<T>(shared_state_);
  }

  // One-shot
  void SetValue(T val) {
    if (!shared_state_) {
      throw NoStateError();
    }

    std::lock_guard guard{shared_state_->mutex_};

    if (shared_state_->result_ready_) {
      throw PromiseAlreadySatisfiedError();
    }

    shared_state_->result_ = typename SharedState<T>::Result(std::move(val));
    shared_state_->result_ready_ = true;
    shared_state_->result_ready_cv_.notify_one();
  }

  // One-shot
  void SetException(std::exception_ptr ptr) {
    if (!shared_state_) {
      throw NoStateError();
    }

    std::lock_guard guard{shared_state_->mutex_};

    if (shared_state_->result_ready_) {
      throw PromiseAlreadySatisfiedError();
    }

    shared_state_->result_ =
        typename SharedState<T>::Result(std::unexpected(std::move(ptr)));
    shared_state_->result_ready_ = true;
    shared_state_->result_ready_cv_.notify_one();
  }

 private:
  std::shared_ptr<SharedState<T>> shared_state_;
};
