#pragma once

#include <memory>

#include "gfx/compute_backend.h"
#include "gfx/render_backend.h"

namespace ndyn::gfx {

class BackendFactory {
 public:
  virtual ~BackendFactory() = default;

  virtual std::unique_ptr<ComputeBackend> createComputeBackend() = 0;
  virtual std::unique_ptr<RenderBackend> createRenderBackend() = 0;
};

}  // namespace ndyn::gfx
