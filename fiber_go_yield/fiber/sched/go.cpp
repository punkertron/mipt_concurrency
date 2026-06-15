#include "go.hpp"

#include <memory>
#include <utility>

#include <exe/fiber/core/fiber.hpp>
#include <exe/fiber/core/scheduler.hpp>

namespace exe::fiber {

void Go(Scheduler& scheduler, Body body) {
  auto fiber = std::make_unique<Fiber>(scheduler, std::move(body));
  Fiber::Schedule(std::move(fiber));
}

void Go(Body body) {
  Go(Fiber::CurrentScheduler(), std::move(body));
}

}  // namespace exe::fiber
