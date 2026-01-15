#pragma once

namespace ndyn::gfx {

class CommandBuffer {
 public:
  virtual ~CommandBuffer() = default;

  virtual void begin() = 0;
  virtual void end() = 0;
};

}  // namespace ndyn::gfx
