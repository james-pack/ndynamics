// clang-format off
#include "glad/gl.h"
// clang-format on

#include "ui/ui_elements.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace ndyn::ui {

void Window::update() {
  static constexpr float PAD{10};
  const ImGuiViewport* viewport = ImGui::GetMainViewport();

  ImVec2 work_pos = viewport->WorkPos;  // Use work area to avoid menu-bar/task-bar, if any!
  ImVec2 work_size = viewport->WorkSize;
  ImVec2 window_pos, window_pos_pivot;
  window_pos.x = work_pos.x + work_size.x - PAD;
  window_pos.y = work_pos.y + PAD;
  window_pos_pivot.x = 1.0f;
  window_pos_pivot.y = 0.0f;
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

  ImGui::SetNextWindowBgAlpha(0.35f);  // Transparent background

  const ImGuiWindowFlags window_flags{
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove};
  ImGui::Begin("##App", nullptr, window_flags);

  for (auto* child : children_) {
    child->update();
  }

  ImGui::End();
}

}  // namespace ndyn::ui
