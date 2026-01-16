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

  virtual std::unique_ptr<RenderPipeline> create_render_pipeline(ShaderModule& vertex,
                                                                 ShaderModule& fragment,
                                                                 PipelineLayout& layout,
                                                                 RenderPass& pass) = 0;

  virtual std::unique_ptr<CommandBuffer> create_command_buffer() = 0;

  virtual std::unique_ptr<Fence> submit(CommandBuffer& cmd) = 0;
};

}  // namespace ndyn::gfx
