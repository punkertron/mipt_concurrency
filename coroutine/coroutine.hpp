#pragma once

#include <sure/context.hpp>
#include <sure/stack/mmap.hpp>

#include <function2/function2.hpp>

class Coroutine : private sure::ITrampoline {
 public:
  class SuspendContext {
    friend class Coroutine;

   public:
    void Suspend() {
      self_->Suspend();
    }

   private:
    explicit SuspendContext(Coroutine* coro)
        : self_(coro) {
    }

   private:
    Coroutine* self_;
  };

 private:
  using Body = fu2::unique_function<void(SuspendContext)>;

 public:
  explicit Coroutine(Body);

  void Resume();
  void Suspend();

  bool IsDone() const;

 private:
  void Run() noexcept override;

 private:
  Body body_;
  sure::stack::GuardedMmapExecutionStack stack_;
  sure::ExecutionContext context_;
  sure::ExecutionContext caller_context_;
  bool done_ = false;
};
