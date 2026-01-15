#pragma once

#include "gfx/pipeline_layout.h"

namespace ndyn::gfx {

class ComputePipeline {
 public:
  virtual ~ComputePipeline() = default;

  virtual PipelineLayout& layout() const = 0;
};

}  // namespace ndyn::gfx
