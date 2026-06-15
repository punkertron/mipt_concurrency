#pragma once

#include "body.hpp"
#include "stack.hpp"

#include <twist/ed/sure/context.hpp>

namespace exe::fiber {

class Coroutine final : private sure::ITrampoline {
 public:
  explicit Coroutine(Body);

  void Resume();
  void Suspend();

  bool IsDone() const;

 private:
  void Run() noexcept override;

 private:
  Body body_;
  Stack stack_;
  twist::ed::sure::ExecutionContext context_;
  twist::ed::sure::ExecutionContext caller_context_;
  bool done_ = false;
};

}  // namespace exe::fiber
