#include "manual_loop.hpp"

#include <limits>
#include <algorithm>

namespace exe::runtime {

void ManualLoop::Submit(task::Task task) {
  queue_.push(std::move(task));
}

// Run tasks

size_t ManualLoop::RunAtMostTasks(size_t limit) {
  size_t completed = 0;

  while (completed < limit && !queue_.empty()) {
    auto task = std::move(queue_.front());
    queue_.pop();
    task();
    ++completed;
  }
  return completed;
}

size_t ManualLoop::RunTasks() {
  return RunAtMostTasks(std::numeric_limits<size_t>::max());
}

bool ManualLoop::IsEmpty() const {
  return queue_.empty();
}

}  // namespace exe::runtime
