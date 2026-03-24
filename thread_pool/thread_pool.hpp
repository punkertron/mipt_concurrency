#pragma once

#include "queue.hpp"
#include "task.hpp"

#include <twist/ed/std/thread.hpp>

#include <vector>

// Fixed-size pool of worker threads

class ThreadPool {
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

  void Submit(Task);

  static ThreadPool* Current();

  void Stop();

 private:
  void Run();

  std::vector<twist::ed::std::thread> threads_;
  UnboundedBlockingQueue<Task> queue_;
  const size_t num_threads_{0};
  bool running_{false};
};
