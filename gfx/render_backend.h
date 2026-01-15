#pragma once

#include <memory>

#include "gfx/buffer.h"
#include "gfx/command_buffer.h"
#include "gfx/fence.h"
#include "gfx/render_pass.h"
#include "gfx/render_pipeline.h"
#include "gfx/shader_module.h"

namespace ndyn::gfx {

class RenderBackend {
 public:
  virtual ~RenderBackend() = default;

  virtual std::unique_ptr<ShaderModule> createVertexShader(const void* bytecode,
                                                           std::size_t size) = 0;

  virtual std::unique_ptr<ShaderModule> createFragmentShader(const void* bytecode,
                                                             std::size_t size) = 0;

  virtual std::unique_ptr<RenderPipeline> createRenderPipeline(ShaderModule& vertex,
                                                               ShaderModule& fragment,
                                                               PipelineLayout& layout,
                                                               RenderPass& pass) = 0;

  virtual std::unique_ptr<CommandBuffer> createCommandBuffer() = 0;

  virtual void draw(CommandBuffer& cmd, RenderPipeline& pipeline, Buffer& vertexBuffer,
                    Buffer* indexBuffer, std::uint32_t vertexOrIndexCount) = 0;

  virtual std::unique_ptr<Fence> submit(CommandBuffer& cmd) = 0;
};

}  // namespace ndyn::gfx
