#pragma once

namespace ndyn::gfx {

/**
 * Fence represents a CPU-visible synchronization point for GPU work.
 * It becomes signaled when all commands submitted together have finished
 * executing on the GPU. Fences are used to deterministically sequence
 * simulation steps, safely read back results, and ensure GPU resources
 * are no longer in use before reuse or destruction.
 */
class Fence {
 public:
  virtual ~Fence() = default;

  virtual void wait() = 0;
  virtual bool is_complete() = 0;
};

}  // namespace ndyn::gfx
