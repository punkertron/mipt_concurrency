#pragma once

#include <exe/fiber/core/body.hpp>
#include <exe/runtime/task/scheduler.hpp>

namespace exe::fiber {

// Considered harmful

void Go(runtime::task::IScheduler&, Body);

void Go(Body);

}  // namespace exe::fiber
