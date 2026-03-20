#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/assist/assert.hpp>

namespace dining {

// Shared resource

class Plate {
 public:
  void Access() {
    // Enter
    [[maybe_unused]] bool exclusive = !accessing_.exchange(true);
    TWIST_ASSERT_M(exclusive, "Mutual exclusion violated");
    // Exit
    accessing_.store(false);
  }

 private:
  twist::ed::std::atomic<bool> accessing_{false};
};

}  // namespace dining
