#pragma once

#include "gfx/pipeline_layout.h"

namespace ndyn::gfx {

class RenderPipeline {
 public:
  virtual ~RenderPipeline() = default;

  virtual PipelineLayout* layout() const = 0;
};

}  // namespace ndyn::gfx
