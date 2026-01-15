#pragma once

#include <memory>

#include "gfx/buffer.h"
#include "gfx/command_buffer.h"
#include "gfx/compute_pipeline.h"
#include "gfx/fence.h"
#include "gfx/shader_module.h"

namespace ndyn::gfx {

/**
 * ComputeBackend provides an abstract interface for performing GPU-based computation.
 * It manages GPU resources such as buffers, shaders, pipelines, and command buffers,
 * allowing the user to schedule and execute compute workloads in a backend-agnostic manner.
 *
 * Usage typically involves several steps: first, buffers are created to hold input,
 * output, and intermediate data. Shaders are then compiled into ShaderModule objects,
 * which are combined with PipelineLayout information to create ComputePipeline instances.
 *
 * Work is recorded using CommandBuffer objects via dispatch() calls, which enqueue
 * compute kernel execution on the GPU but do not execute them immediately. Once
 * command recording is complete, submit() is called to send the command buffer to
 * the GPU for execution. submit() returns a Fence object, which can be used to wait
 * for completion (blocking) or to poll for completion with isComplete() (non-blocking).
 *
 * Multiple command buffers can be recorded and submitted in parallel, allowing
 * multiple in-flight submissions. dispatch() may be called on a command buffer at
 * any time before submission, and submit() may be called on multiple command buffers
 * while earlier fences are still pending, as long as backend resource dependencies
 * are respected.
 *
 * The separation of dispatch() and submit() allows the CPU to prepare work ahead of
 * GPU execution, while fences provide deterministic control over when GPU computations
 * are considered complete. This model ensures safe and efficient interaction between
 * simulation and visualization pipelines, and enables backend implementations using
 * Vulkan, WebGPU, or other GPU APIs without changing the application code.
 */
class ComputeBackend {
 public:
  virtual ~ComputeBackend() = default;

  virtual std::unique_ptr<Buffer> create_buffer(std::size_t size, BufferUsage usage) = 0;

  virtual std::unique_ptr<ShaderModule> create_compute_shader(const void* bytecode,
                                                              std::size_t size) = 0;

  virtual std::unique_ptr<ComputePipeline> create_compute_pipeline(ShaderModule& shader,
                                                                   PipelineLayout& layout) = 0;

  virtual std::unique_ptr<CommandBuffer> create_command_buffer() = 0;

  /**
   * Records a compute dispatch into the provided command buffer using the given
   * compute pipeline. This method only enqueues work into the command buffer;
   * the dispatched work will not execute on the GPU until the command buffer
   * is submitted via submit().
   */
  virtual void dispatch(CommandBuffer& cmd, ComputePipeline& pipeline, std::uint32_t x,
                        std::uint32_t y, std::uint32_t z) = 0;

  /**
   * Submits a previously recorded command buffer for execution on the GPU and
   * returns a Fence representing completion of all recorded work. Any dispatch()
   * calls recorded into the command buffer become visible to the GPU only after
   * submit() is called, and the returned Fence can be used to synchronize CPU-side
   * logic with the completion of those dispatches.
   */
  virtual std::unique_ptr<Fence> submit(CommandBuffer& cmd) = 0;
};

}  // namespace ndyn::gfx
