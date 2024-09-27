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
  CenterPane();

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
  LeftRightPane();

  // Update, called once per frame to update any gui elements. Not called when paused.
  void update() override;

  void add_left_child(UiElement& child) { left_children_.push_back(&child); }
  void add_right_child(UiElement& child) { right_children_.push_back(&child); }
};

}  // namespace ndyn::ui
