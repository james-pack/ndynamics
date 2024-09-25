#include "ui/keyboard_shortcuts.h"

#include "imgui.h"

namespace ndyn::ui {

Shortcuts& Shortcuts::global_shortcuts() {
  static Shortcuts global{};
  return global;
}

void Shortcuts::render_key_binding_help_text(bool* is_open) {
  ImGui::Begin("Key Bindings", is_open);

  if (ImGui::BeginTable("##Key Bindings List", /* num columns*/ 2)) {
    for (const auto& [chord, text] : help_text_) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(/* first column */ 0);
      ImGui::Text("%d", chord);
      ImGui::TableSetColumnIndex(/* second column */ 1);
      ImGui::Text("%s", text.c_str());
    }

    ImGui::EndTable();
  }

  ImGui::End();
}

void bind_key(ImGuiKeyChord chord, std::string_view help_text, const ShortcutCallback& callback) {
  Shortcuts::global_shortcuts().bind_key(chord, help_text, callback);
}

}  // namespace ndyn::ui
