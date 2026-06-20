#pragma once

#include <exe/util/queue.hpp>
#include <exe/runtime/task/task.hpp>
#include <exe/runtime/task/scheduler.hpp>
#include <twist/ed/std/thread.hpp>

#include <vector>

namespace exe::runtime {

class ThreadPool final : public task::IScheduler {
 public:
  explicit ThreadPool(size_t num_threads);
  ~ThreadPool();

  // Non-copyable
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // Non-movable
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  void Start();

  // task::IScheduler
  void Submit(task::Task) override;

  static ThreadPool* Current();

  void Stop();

 private:
  void Run();

  std::vector<twist::ed::std::thread> threads_;
  UnboundedBlockingQueue<task::Task> queue_;
  const size_t num_threads_{0};
  bool running_{false};
};

}  // namespace exe::runtime
