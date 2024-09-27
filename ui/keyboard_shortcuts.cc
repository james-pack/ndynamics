#include "ui/keyboard_shortcuts.h"

#include "imgui.h"
#include "ui/imgui_utils.h"

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
      ImGui::Text("%s", key_chord_to_string(chord).c_str());
      ImGui::TableSetColumnIndex(/* second column */ 1);
      ImGui::Text("%s", text.c_str());
    }

    ImGui::EndTable();
  }

  ImGui::End();
}

void Shortcuts::bind_key(ImGuiKeyChord chord, std::string_view help_text,
                         const ShortcutCallback& callback) {
  chords_.push_back(chord);
  callbacks_.insert({chord, callback});
  help_text_.push_back({chord, std::string{help_text}});
}

void Shortcuts::unbind_key(ImGuiKeyChord chord) {
  for (auto iter = chords_.begin(); iter != chords_.end(); ++iter) {
    if (*iter == chord) {
      chords_.erase(iter);
      break;
    }
  }

  {
    auto iter{callbacks_.find(chord)};
    if (iter != callbacks_.end()) {
      callbacks_.erase(iter);
    }
  }

  for (auto iter = help_text_.begin(); iter != help_text_.end(); ++iter) {
    if (iter->first == chord) {
      help_text_.erase(iter);
      break;
    }
  }
}

void bind_key(ImGuiKeyChord chord, std::string_view help_text, const ShortcutCallback& callback) {
  Shortcuts::global_shortcuts().bind_key(chord, help_text, callback);
}

void unbind_key(ImGuiKeyChord chord) { Shortcuts::global_shortcuts().unbind_key(chord); }

}  // namespace ndyn::ui
