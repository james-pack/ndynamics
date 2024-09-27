#pragma once

#include <vector>

namespace ndyn::ui {

/**
 * Mostly standard class hierarchy for UI elements.
 */
class UiElement {
 public:
  virtual ~UiElement() = default;

  // Called when this UI element is loaded as part of the active scene.
  virtual void handle_loading() {}
  // Called when this UI element is being unloaded.
  virtual void handle_unloading() {}

  // Update, called once per frame to update any gui elements. Not called when paused.
  virtual void update() {}
};

/**
 * Top-level UI element that holds other UI elements.
 */
class Pane : public UiElement {
 public:
  virtual ~Pane() = 0;
};

/**
 * Pane that renders its children in the center of the window.
 */
class CenterPane final : public Pane {
 private:
  std::vector<UiElement*> children_{};

  bool show_children_{true};

 public:
  // Called when this UI element is loaded as part of the active scene.
  void handle_loading() override;
  // Called when this UI element is being unloaded.
  void handle_unloading() override;

  // Update, called once per frame to update any gui elements. Not called when paused.
  void update() override;

  void add_child(UiElement& child) { children_.push_back(&child); }
};

/**
 * Pane that renders children on the left and right sides of the window.
 */
class LeftRightPane final : public Pane {
 private:
  std::vector<UiElement*> left_children_{};
  std::vector<UiElement*> right_children_{};

  bool show_left_children_{true};
  bool show_right_children_{true};

 public:
  // Called when this UI element is loaded as part of the active scene.
  void handle_loading() override;
  // Called when this UI element is being unloaded.
  void handle_unloading() override;

  // Update, called once per frame to update any gui elements. Not called when paused.
  void update() override;

  void add_left_child(UiElement& child) { left_children_.push_back(&child); }
  void add_right_child(UiElement& child) { right_children_.push_back(&child); }
};

}  // namespace ndyn::ui
