#pragma once

#include <memory>

#include "body.hpp"
#include "coroutine.hpp"

#include <exe/runtime/task/scheduler.hpp>

namespace exe::fiber {

// Fiber = Stackful coroutine x Scheduler

class Fiber {
 public:
  Fiber(runtime::task::IScheduler&, Body);

  static Fiber& Self();
  static runtime::task::IScheduler& CurrentScheduler();

  static void Schedule(std::unique_ptr<Fiber>);
  void Yield();

 private:
  runtime::task::IScheduler& GetScheduler() const;

 private:
  runtime::task::IScheduler& scheduler_;
  Coroutine coro_;
};

}  // namespace exe::fiber
