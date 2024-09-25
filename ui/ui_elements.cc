// clang-format off
#include "glad/gl.h"
// clang-format on

#include "ui/ui_elements.h"

#include "glog/logging.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ui/imgui_utils.h"
#include "ui/keyboard_shortcuts.h"

namespace ndyn::ui {

Window::Window() {
  bind_key(ImGuiKey_H, "Help", [this](ImGuiKeyChord) { show_help_text_ = not show_help_text_; });
  bind_key(/* question mark key: "?" */ ImGuiKey_Slash | ImGuiMod_Shift, "Help",
           [this](ImGuiKeyChord) { show_help_text_ = not show_help_text_; });

  bind_key(ImGuiKey_LeftBracket, "Toggle display of left pane",
           [this](ImGuiKeyChord) { show_left_children_ = not show_left_children_; });
  bind_key(ImGuiKey_RightBracket, "Toggle display of right pane",
           [this](ImGuiKeyChord) { show_right_children_ = not show_right_children_; });
}

void Window::update() {
  // Fraction of the horizontal window size that will be used for the UI panes.
  static constexpr float UI_FRACTION_HORIZONTAL{0.3};

  static constexpr ImVec2 WINDOW_POS_PIVOT_LEFT{0.f, 0.f};
  static constexpr ImVec2 WINDOW_POS_PIVOT_RIGHT{1.f, 0.f};
  static constexpr ImVec2 WINDOW_POS_PIVOT_CENTER{0.5f, 0.f};

  const ImGuiViewport* viewport = ImGui::GetMainViewport();

  const ImVec2 work_pos = viewport->WorkPos;  // Use work area to avoid menu-bar/task-bar, if any!
  const ImVec2 work_size = viewport->WorkSize;

  static constexpr ImGuiWindowFlags WINDOW_FLAGS{
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove};

  if (show_left_children_ && !left_children_.empty()) {  // Add left children to UI
    ImVec2 window_size, window_pos;
    window_size.x = work_size.x * UI_FRACTION_HORIZONTAL;
    window_size.y = 0.f;  // Auto-fit the y-axis.

    // We are going to right-justify this next window.
    window_pos.x = work_pos.x;
    window_pos.y = work_pos.y;

    VLOG(6) << "work_pos: " << work_pos << ", work_size: " << work_size
            << ", window_pos: " << window_pos;

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, WINDOW_POS_PIVOT_LEFT);
    ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.1f);  // Transparent background

    ImGui::Begin("##Window Left", nullptr, WINDOW_FLAGS);

    for (auto* child : left_children_) {
      child->update();
    }

    ImGui::End();
  }

  if (show_right_children_ && !right_children_.empty()) {  // Add right children to UI
    ImVec2 window_size, window_pos;
    window_size.x = work_size.x * UI_FRACTION_HORIZONTAL;
    window_size.y = 0.f;  // Auto-fit the y-axis.

    // We are going to right-justify this next window.
    window_pos.x = work_pos.x + work_size.x;
    window_pos.y = work_pos.y;

    VLOG(6) << "work_pos: " << work_pos << ", work_size: " << work_size
            << ", window_pos: " << window_pos;

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, WINDOW_POS_PIVOT_RIGHT);
    ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.1f);  // Transparent background

    ImGui::Begin("##Window Right", nullptr, WINDOW_FLAGS);

    for (auto* child : right_children_) {
      child->update();
    }

    ImGui::End();
  }

  if (show_help_text_) {
    ImVec2 window_size, window_pos;
    window_size.x = 0.f;  // Auto-fit the x-axis.
    window_size.y = 0.f;  // Auto-fit the y-axis.

    // We are going to center this next window horizontally.
    window_pos.x = work_pos.x + work_size.x / 2;
    window_pos.y = work_pos.y;

    VLOG(6) << "work_pos: " << work_pos << ", work_size: " << work_size
            << ", window_pos: " << window_pos;

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, WINDOW_POS_PIVOT_CENTER);
    ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

    Shortcuts::global_shortcuts().render_key_binding_help_text(&show_help_text_);
  }
}

}  // namespace ndyn::ui
