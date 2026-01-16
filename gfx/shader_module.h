#pragma once

namespace ndyn::gfx {

enum class ShaderUsage {
  COMPUTE,
  VERTEX,
  FRAGMENT,
  UNIVERSAL,
};

class ShaderModule {
 public:
  virtual ~ShaderModule() = default;

  virtual ShaderUsage usage() const = 0;
};

}  // namespace ndyn::gfx
