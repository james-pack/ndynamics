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

  virtual std::unique_ptr<ShaderModule> create_vertex_shader(const void* bytecode,
                                                             std::size_t size) = 0;

  virtual std::unique_ptr<ShaderModule> create_fragment_shader(const void* bytecode,
                                                               std::size_t size) = 0;

  virtual std::unique_ptr<RenderPipeline> create_render_pipeline(ShaderModule& vertex,
                                                                 ShaderModule& fragment,
                                                                 PipelineLayout& layout,
                                                                 RenderPass& pass) = 0;

  virtual std::unique_ptr<CommandBuffer> create_command_buffer() = 0;

  virtual void dispatch(CommandBuffer& cmd, RenderPipeline& pipeline, Buffer& vertex_buffer,
                        Buffer* index_buffer, std::uint32_t vertexOrIndexCount) = 0;

  virtual std::unique_ptr<Fence> submit(CommandBuffer& cmd) = 0;
};

}  // namespace ndyn::gfx
