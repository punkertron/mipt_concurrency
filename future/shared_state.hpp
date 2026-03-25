#pragma once

#include <twist/ed/std/condition_variable.hpp>
#include <twist/ed/std/mutex.hpp>

#include <exception>
#include <expected>

template <class T>
struct SharedState {
  using Result = std::expected<T, std::exception_ptr>;

  twist::ed::std::mutex mutex_;
  twist::ed::std::condition_variable result_ready_cv_;

  bool future_valid_{true};
  bool future_retrieved_{false};
  bool result_ready_{false};
  Result result_{std::unexpected(std::exception_ptr{})};
};
