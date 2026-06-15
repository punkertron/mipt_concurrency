#pragma once

#include <memory>

#include "body.hpp"
#include "coroutine.hpp"
#include "scheduler.hpp"

namespace exe::fiber {

// Fiber = Stackful coroutine x Scheduler

class Fiber {
 public:
  Fiber(Scheduler&, Body);

  static Fiber& Self();
  static Scheduler& CurrentScheduler();

  static void Schedule(std::unique_ptr<Fiber>);
  void Yield();

 private:
  Scheduler& GetScheduler() const;

 private:
  Scheduler& scheduler_;
  Coroutine coro_;
};

}  // namespace exe::fiber
