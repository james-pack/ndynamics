#pragma once

#include <vector>

namespace ndyn::ui {

/**
 * Mostly standard class hierarchy for UI elements.
 */
class UiElement {
 public:
  virtual ~UiElement() = default;

  // Update, called once per frame to update any gui elements. Not called when paused.
  virtual void update() {}
};

class Window final : public UiElement {
 private:
  std::vector<UiElement*> children_{};

 public:
  // Update, called once per frame to update any gui elements. Not called when paused.
  void update() override;

  void add_child(UiElement& child) { children_.push_back(&child); }
};

}  // namespace ndyn::ui
