#include "fiber.hpp"

#include <cassert>
#include <utility>

#include <twist/ed/static/thread_local/ptr.hpp>

namespace exe::fiber {

namespace {
TWISTED_STATIC_THREAD_LOCAL_PTR(Fiber, current_fiber);
}

Fiber::Fiber(Scheduler& scheduler, Body body)
    : scheduler_(scheduler),
      coro_(std::move(body)) {
}

Fiber& Fiber::Self() {
  assert(current_fiber != nullptr);
  return *current_fiber;
}

Scheduler& Fiber::CurrentScheduler() {
  assert(current_fiber != nullptr);
  return Self().GetScheduler();
}

void Fiber::Schedule(std::unique_ptr<Fiber> fiber) {
  auto& scheduler = fiber->GetScheduler();
  scheduler.Submit([fiber = std::move(fiber)]() mutable {
    current_fiber = fiber.get();
    fiber->coro_.Resume();
    current_fiber = nullptr;
    if (!fiber->coro_.IsDone()) {
      Fiber::Schedule(std::move(fiber));
    }
  });
}

void Fiber::Yield() {
  coro_.Suspend();
}

Scheduler& Fiber::GetScheduler() const {
  return scheduler_;
}

}  // namespace exe::fiber
