#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "imgui.h"

namespace ndyn::ui {

using ShortcutCallback = std::function<void(ImGuiKeyChord chord)>;

class Shortcuts final {
 private:
  std::vector<ImGuiKeyChord> chords_{};
  std::unordered_map<ImGuiKeyChord, ShortcutCallback> callbacks_{};
  std::vector<std::pair<ImGuiKeyChord, std::string>> help_text_{};

 public:
  void bind_key(ImGuiKeyChord chord, std::string_view help_text, const ShortcutCallback& callback);
  void unbind_key(ImGuiKeyChord chord);

  void process_key_presses() const {
    for (auto chord : chords_) {
      if (ImGui::IsKeyChordPressed(chord)) {
        callbacks_.at(chord)(chord);
      }
    }
  }

  void render_key_binding_help_text(bool* is_open);

  static Shortcuts& global_shortcuts();
};

void bind_key(ImGuiKeyChord chord, std::string_view help_text, const ShortcutCallback& callback);
void unbind_key(ImGuiKeyChord chord);

}  // namespace ndyn::ui
