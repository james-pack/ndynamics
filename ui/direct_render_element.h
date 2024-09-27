#pragma once

namespace ndyn::ui {

/**
 * Class hierarchy for elements that are directly rendered on screen through OpenGL calls.
 */
class DirectRenderElement {
 public:
  virtual ~DirectRenderElement() = default;

  // Called when this UI element is loaded as part of the active scene.
  virtual void handle_loading() = 0;
  // Called when this UI element is being unloaded.
  virtual void handle_unloading() = 0;

  // Update, called once per frame to update any visible elements. Not called when paused.
  virtual void update() {}
};

}  // namespace ndyn::ui
