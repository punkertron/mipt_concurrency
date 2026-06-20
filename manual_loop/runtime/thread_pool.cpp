#include "thread_pool.hpp"

#include <cassert>
#include <utility>

#include <twist/ed/static/thread_local/ptr.hpp>

namespace exe::runtime {

namespace {
TWISTED_STATIC_THREAD_LOCAL_PTR(ThreadPool, current_pool);
}

ThreadPool::ThreadPool(size_t num_threads)
    : num_threads_{num_threads} {
  assert(num_threads_ > 0);
}

void ThreadPool::Start() {
  running_ = true;
  threads_.reserve(num_threads_);
  for (size_t i = 0; i < num_threads_; ++i) {
    threads_.emplace_back([this]() {
      Run();
    });
  }
}

ThreadPool::~ThreadPool() {
  assert(!running_);
}

void ThreadPool::Submit(task::Task task) {
  queue_.Push(std::move(task));
}

ThreadPool* ThreadPool::Current() {
  return current_pool;
}

void ThreadPool::Stop() {
  running_ = false;
  queue_.Close();

  // wait all task
  for (auto& thread : threads_) {
    thread.join();
  }
}

void ThreadPool::Run() {
  current_pool = this;
  while (true) {
    auto task = queue_.Pop();
    if (!task) {
      break;
    }
    (*task)();
  }
  current_pool = nullptr;
}

}  // namespace exe::runtime
