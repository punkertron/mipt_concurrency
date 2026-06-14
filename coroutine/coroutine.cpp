#include "coroutine.hpp"

#include <cassert>
#include <cstdlib>
#include <exception>
#include <utility>

#include <sure/stack/mmap.hpp>

static sure::stack::GuardedMmapExecutionStack AllocateStack();

Coroutine::Coroutine(Body body)
    : body_(std::move(body)),
      stack_(AllocateStack()) {
  context_.Setup(stack_.MutView(), this);
}

void Coroutine::Resume() {
  assert(!IsDone());
  caller_context_.SwitchTo(context_);
}

void Coroutine::Suspend() {
  context_.SwitchTo(caller_context_);
}

bool Coroutine::IsDone() const {
  return done_;
}

void Coroutine::Run() noexcept {
  try {
    body_(SuspendContext{this});
  } catch (...) {
    std::terminate();
  }
  done_ = true;
  context_.ExitTo(caller_context_);

  std::abort();  // Unreachable
}

static sure::stack::GuardedMmapExecutionStack AllocateStack() {
  static constexpr size_t kSize = 1024 * 1024;
  return sure::stack::GuardedMmapExecutionStack::AllocateAtLeastBytes(kSize);
}
