#pragma once

namespace ndyn::ui {

/**
 * Class hierarchy for elements that are directly rendered on screen through OpenGL calls.
 */
class DirectRenderElement {
 public:
  virtual ~DirectRenderElement() = default;

  // Update, called once per frame to update any visible elements. Not called when paused.
  virtual void update() {}
};

}  // namespace ndyn::ui
