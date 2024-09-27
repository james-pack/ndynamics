// clang-format off
#include "glad/gl.h"
// clang-format on

#include "ui/app.h"

#include <chrono>
#include <string>
#include <thread>

#include "GLFW/glfw3.h"
#include "glog/logging.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "ui/imgui_utils.h"
#include "ui/keyboard_shortcuts.h"

namespace ndyn::ui {

void style_colors_app() {
  static const ImVec4 bg_dark = ImVec4(0.15f, 0.16f, 0.21f, 1.00f);
  static const ImVec4 bg_mid = ImVec4(0.20f, 0.21f, 0.27f, 1.00f);
  static const ImVec4 accent_dark = ImVec4(0.292f, 0.360f, 0.594f, 1.000f);
  static const ImVec4 accent_light = ImVec4(0.409f, 0.510f, 0.835f, 1.000f);
  static const ImVec4 active = ImVec4(0.107f, 0.118f, 0.157f, 1.000f);
  static const ImVec4 attention = ImVec4(0.821f, 1.000f, 0.000f, 1.000f);

  auto &style = ImGui::GetStyle();
  style.WindowPadding = {6, 6};
  style.FramePadding = {6, 3};
  style.CellPadding = {6, 3};
  style.ItemSpacing = {6, 6};
  style.ItemInnerSpacing = {6, 6};
  style.ScrollbarSize = 16;
  style.GrabMinSize = 8;
  style.WindowBorderSize = style.ChildBorderSize = style.PopupBorderSize = style.TabBorderSize = 0;
  style.FrameBorderSize = 1;
  style.WindowRounding = style.ChildRounding = style.PopupRounding = style.ScrollbarRounding =
      style.GrabRounding = style.TabRounding = 4;

  ImVec4 *colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text] = ImVec4(0.89f, 0.89f, 0.92f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.38f, 0.45f, 0.64f, 1.00f);
  colors[ImGuiCol_WindowBg] = bg_mid;
  colors[ImGuiCol_ChildBg] = ImVec4(0.20f, 0.21f, 0.27f, 0.00f);
  colors[ImGuiCol_PopupBg] = bg_mid;
  colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.06f);
  colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.02f);
  colors[ImGuiCol_FrameBgHovered] = accent_light;
  colors[ImGuiCol_FrameBgActive] = active;
  colors[ImGuiCol_TitleBg] = accent_dark;
  colors[ImGuiCol_TitleBgActive] = accent_dark;
  colors[ImGuiCol_TitleBgCollapsed] = accent_dark;
  colors[ImGuiCol_MenuBarBg] = accent_dark;
  colors[ImGuiCol_ScrollbarBg] = bg_mid;
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.89f, 0.89f, 0.93f, 0.27f);
  colors[ImGuiCol_ScrollbarGrabHovered] = accent_light;
  colors[ImGuiCol_ScrollbarGrabActive] = active;
  colors[ImGuiCol_CheckMark] = accent_dark;
  colors[ImGuiCol_SliderGrab] = accent_dark;
  colors[ImGuiCol_SliderGrabActive] = accent_light;
  colors[ImGuiCol_Button] = accent_dark;
  colors[ImGuiCol_ButtonHovered] = accent_light;
  colors[ImGuiCol_ButtonActive] = active;
  colors[ImGuiCol_Header] = accent_dark;
  colors[ImGuiCol_HeaderHovered] = accent_light;
  colors[ImGuiCol_HeaderActive] = active;
  colors[ImGuiCol_Separator] = accent_dark;
  colors[ImGuiCol_SeparatorHovered] = accent_light;
  colors[ImGuiCol_SeparatorActive] = active;
  colors[ImGuiCol_ResizeGrip] = accent_dark;
  colors[ImGuiCol_ResizeGripHovered] = accent_light;
  colors[ImGuiCol_ResizeGripActive] = active;
  colors[ImGuiCol_Tab] = ImVec4(1.00f, 1.00f, 1.00f, 0.02f);
  colors[ImGuiCol_TabHovered] = accent_light;
  colors[ImGuiCol_TabActive] = accent_dark;
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
  colors[ImGuiCol_TabUnfocusedActive] = active;
  colors[ImGuiCol_PlotLines] = accent_light;
  colors[ImGuiCol_PlotLinesHovered] = active;
  colors[ImGuiCol_PlotHistogram] = accent_light;
  colors[ImGuiCol_PlotHistogramHovered] = active;
  colors[ImGuiCol_TableHeaderBg] = accent_dark;
  colors[ImGuiCol_TableBorderStrong] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
  colors[ImGuiCol_TableBorderLight] = ImVec4(1.00f, 1.00f, 1.00f, 0.02f);
  colors[ImGuiCol_TableRowBg] = ImVec4(0, 0, 0, 0);
  colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.02f);
  colors[ImGuiCol_TextSelectedBg] = accent_light;
  colors[ImGuiCol_DragDropTarget] = attention;
  colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
#ifdef IMGUI_HAS_DOCK
  colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);
  colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
#endif

  ImPlot::StyleColorsAuto();

  ImVec4 *pcolors = ImPlot::GetStyle().Colors;
  pcolors[ImPlotCol_PlotBg] = bg_dark;
  pcolors[ImPlotCol_PlotBorder] = ImVec4(0, 0, 0, 0);
  pcolors[ImPlotCol_Selection] = attention;
  pcolors[ImPlotCol_Crosshairs] = colors[ImGuiCol_Text];

  // ImPlot::GetStyle().DigitalBitHeight = 20;

  auto &pstyle = ImPlot::GetStyle();
  pstyle.PlotPadding = pstyle.LegendPadding = {12, 12};
  pstyle.LabelPadding = pstyle.LegendInnerPadding = {6, 6};
  pstyle.LegendSpacing = {10, 2};
  pstyle.AnnotationPadding = {4, 2};

  const ImU32 Dracula[] = {4288967266, 4285315327, 4286315088, 4283782655, 4294546365,
                           4287429361, 4291197439, 4294830475, 4294113528, 4284106564};
  ImPlot::GetStyle().Colormap = ImPlot::AddColormap("Dracula", Dracula, 10);
}

static void glfw_error_callback(int error, const char *description) {
  LOG(ERROR) << "GLFW Error " << error << ": " << description;
}

App::App(std::string title, size_t width, size_t height) : default_window_title_(title) {
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) abort();

  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);          // 3.0+ only

  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);
  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  const bool fullscreen{width == 0 && height == 0};
  if (width == 0) {
    width = mode->width;
  }
  if (height == 0) {
    height = mode->height;
  }
  // Scaling the window to the monitor helps with readability on high resolution monitors.
  // Otherwise, the text and other renderings can be too small on 4K and higher resolution
  // monitors.
  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);

  // Create window with graphics context
  if (fullscreen) {
    window_ = glfwCreateWindow(width, height, default_window_title_.c_str(), monitor, nullptr);
  } else {
    window_ = glfwCreateWindow(width, height, default_window_title_.c_str(), nullptr, nullptr);
  }
  if (window_ == nullptr) {
    LOG(FATAL) << "Failed to initialize GLFW window!";
  }

  glfwMakeContextCurrent(window_);
  glfwSwapInterval(0);

  // Initialize OpenGL loader
  int version = gladLoadGL(glfwGetProcAddress);
  LOG(INFO) << "glfw version: " << version;
  if (version == 0) {
    LOG(FATAL) << "Failed to initialize OpenGL loader!";
  }

  // Add the GPU details to the window title.
  const GLubyte *renderer = glGetString(GL_RENDERER);
  default_window_title_.append(" - ").append(reinterpret_cast<const char *>(renderer));
  glfwSetWindowTitle(window_, default_window_title_.c_str());

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();

  ImGuiIO &io = ImGui::GetIO();

  if (!fullscreen) {
    ImVec2 monitor_scale;
    glfwGetMonitorContentScale(monitor, &monitor_scale.x, &monitor_scale.y);
    if (monitor_scale.x >= monitor_scale.y) {
      io.FontGlobalScale = monitor_scale.x;
    } else {
      io.FontGlobalScale = monitor_scale.y;
    }
  }

  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  clear_color_ = ImVec4(0.01f, 0.05f, 0.1f, 1.00f);
  style_colors_app();

  io.IniFilename = nullptr;
  io.LogFilename = nullptr;

  bind_key(ImGuiKey_Space, "Pause/unpause UI rendering", [this](ImGuiKeyChord) { invert_pause(); });
  bind_key(ImGuiKey_P, "Pause/unpause UI rendering", [this](ImGuiKeyChord) { invert_pause(); });

  bind_key(ImGuiKey_Escape, "Quit the application", [this](ImGuiKeyChord) {
    request_close();
    pause();
  });

  bind_key(ImGuiKey_Q, "Quit the application", [this](ImGuiKeyChord) {
    request_close();
    pause();
  });

  bind_key(ImGuiKey_F1, "Switch to scene bank 1", [this](ImGuiKeyChord) { goto_scene_bank(0); });
  bind_key(ImGuiKey_F2, "Switch to scene bank 2", [this](ImGuiKeyChord) { goto_scene_bank(1); });
  bind_key(ImGuiKey_F3, "Switch to scene bank 3", [this](ImGuiKeyChord) { goto_scene_bank(2); });
  bind_key(ImGuiKey_F4, "Switch to scene bank 4", [this](ImGuiKeyChord) { goto_scene_bank(3); });
  bind_key(ImGuiKey_F5, "Switch to scene bank 5", [this](ImGuiKeyChord) { goto_scene_bank(4); });
  bind_key(ImGuiKey_F6, "Switch to scene bank 6", [this](ImGuiKeyChord) { goto_scene_bank(5); });
  bind_key(ImGuiKey_F7, "Switch to scene bank 7", [this](ImGuiKeyChord) { goto_scene_bank(6); });
  bind_key(ImGuiKey_F8, "Switch to scene bank 8", [this](ImGuiKeyChord) { goto_scene_bank(7); });
  bind_key(ImGuiKey_F9, "Switch to scene bank 9", [this](ImGuiKeyChord) { goto_scene_bank(8); });
  bind_key(ImGuiKey_F10, "Switch to scene bank 10", [this](ImGuiKeyChord) { goto_scene_bank(9); });

  bind_key(ImGuiKey_F11, "Switch to prev scene bank",
           [this](ImGuiKeyChord) { goto_prev_scene_bank(); });
  bind_key(ImGuiKey_F12, "Switch to next scene bank",
           [this](ImGuiKeyChord) { goto_next_scene_bank(); });

  bind_key(ImGuiKey_1, "Load scene 1 in current bank", [this](ImGuiKeyChord) { load_scene(0); });
  bind_key(ImGuiKey_2, "Load scene 2 in current bank", [this](ImGuiKeyChord) { load_scene(1); });
  bind_key(ImGuiKey_3, "Load scene 3 in current bank", [this](ImGuiKeyChord) { load_scene(2); });
  bind_key(ImGuiKey_4, "Load scene 4 in current bank", [this](ImGuiKeyChord) { load_scene(3); });
  bind_key(ImGuiKey_5, "Load scene 5 in current bank", [this](ImGuiKeyChord) { load_scene(4); });
  bind_key(ImGuiKey_6, "Load scene 6 in current bank", [this](ImGuiKeyChord) { load_scene(5); });
  bind_key(ImGuiKey_7, "Load scene 7 in current bank", [this](ImGuiKeyChord) { load_scene(6); });
  bind_key(ImGuiKey_8, "Load scene 8 in current bank", [this](ImGuiKeyChord) { load_scene(7); });
  bind_key(ImGuiKey_9, "Load scene 9 in current bank", [this](ImGuiKeyChord) { load_scene(8); });
  bind_key(ImGuiKey_0, "Load scene 10 in current bank", [this](ImGuiKeyChord) { load_scene(9); });

  bind_key(ImGuiKey_H, "Help", [this](ImGuiKeyChord) { show_help_text_ = not show_help_text_; });
  bind_key(/* question mark key: "?" */ ImGuiKey_Slash | ImGuiMod_Shift, "Help",
           [this](ImGuiKeyChord) { show_help_text_ = not show_help_text_; });
}

App::~App() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
  glfwDestroyWindow(window_);
  glfwTerminate();
}

void App::show_help_text() {
  const ImGuiViewport *viewport = ImGui::GetMainViewport();

  const ImVec2 work_pos = viewport->WorkPos;  // Use work area to avoid menu-bar/task-bar, if any!
  const ImVec2 work_size = viewport->WorkSize;

  ImVec2 window_size, window_pos;
  window_size.x = 0.f;  // Auto-fit the x-axis.
  window_size.y = 0.f;  // Auto-fit the y-axis.

  // We are going to center this next window horizontally.
  window_pos.x = work_pos.x + work_size.x / 2;
  window_pos.y = work_pos.y;

  VLOG(6) << "work_pos: " << work_pos << ", work_size: " << work_size
          << ", window_pos: " << window_pos;

  static constexpr ImVec2 WINDOW_POS_PIVOT_CENTER{0.5f, 0.f};
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, WINDOW_POS_PIVOT_CENTER);
  ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

  Shortcuts::global_shortcuts().render_key_binding_help_text(&show_help_text_);
}

void App::render_missing_scene_ui() {
  const ImGuiViewport *viewport = ImGui::GetMainViewport();

  const ImVec2 work_pos = viewport->WorkPos;  // Use work area to avoid menu-bar/task-bar, if any!
  const ImVec2 work_size = viewport->WorkSize;

  ImVec2 window_size, window_pos;
  window_size.x = 0.f;  // Auto-fit the x-axis.
  window_size.y = 0.f;  // Auto-fit the y-axis.

  // We are going to center this next window horizontally, but with the text at the bottom.
  window_pos.x = work_pos.x + work_size.x / 2;
  window_pos.y = work_pos.y + work_size.y;

  VLOG(6) << "work_pos: " << work_pos << ", work_size: " << work_size
          << ", window_pos: " << window_pos;

  static constexpr ImVec2 WINDOW_POS_PIVOT_CENTER_BOTTOM{0.5f, 1.f};
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, WINDOW_POS_PIVOT_CENTER_BOTTOM);
  ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

  static constexpr ImGuiWindowFlags WINDOW_FLAGS{
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove};

  ImGui::Begin("##No Scene Available", nullptr, WINDOW_FLAGS);

  ImGui::Text("No Scene Available");
  ImGui::Text("(Scene Bank: %ld)", current_bank_ + 1);

  ImGui::End();
}

void App::run() {
  using namespace std::chrono_literals;

  if (current_scene_ == nullptr) {
    load_scene(0);
  }

  while (!glfwWindowShouldClose(window_) && !close_requested()) {
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    Shortcuts::global_shortcuts().process_key_presses();

    if (current_scene_ == nullptr) {
      render_missing_scene_ui();

      if (show_help_text_) {
        show_help_text();
      }

      // Rendering
      ImGui::Render();

      int display_w, display_h;
      glfwGetFramebufferSize(window_, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glClearColor(clear_color_.x, clear_color_.y, clear_color_.z, clear_color_.w);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      glfwSwapBuffers(window_);

      // Since we are not doing much work, we add a tiny sleep so the CPU doesn't waste energy.
      std::this_thread::sleep_for(50ms);

      continue;
    }

    current_scene_->update_models();

    if (!is_paused()) {
      current_scene_->update_ui();

      if (show_help_text_) {
        show_help_text();
      }

      // Rendering
      ImGui::Render();

      int display_w, display_h;
      glfwGetFramebufferSize(window_, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glClearColor(clear_color_.x, clear_color_.y, clear_color_.z, clear_color_.w);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      current_scene_->update_direct_render_elements();

      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      glfwSwapBuffers(window_);
    } else {
      // Since we are not doing much work, we add a tiny sleep so the CPU doesn't waste energy.
      std::this_thread::sleep_for(50ms);
    }

    VLOG(1) << "Frame rate: " << ImGui::GetIO().Framerate << " fps";
  }
}

}  // namespace ndyn::ui
