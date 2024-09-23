// clang-format off
#include "glad/gl.h"
// clang-format on

#include "ui/app.h"

#include <chrono>
#include <filesystem>
#include <string>
#include <thread>

#include "glog/logging.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "implot.h"
#include "io/utils.h"
#include "ui/imgui_utils.h"

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

App::App(std::string title, size_t width, size_t height) {
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
    window_ = glfwCreateWindow(width, height, title.c_str(), monitor, nullptr);
  } else {
    window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
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
  title += " - ";
  title += reinterpret_cast<const char *>(renderer);
  glfwSetWindowTitle(window_, title.c_str());

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();

  ImVec2 monitor_scale;
  glfwGetMonitorContentScale(monitor, &monitor_scale.x, &monitor_scale.y);
  ImGuiIO &io = ImGui::GetIO();
  if (monitor_scale.x >= monitor_scale.y) {
    io.FontGlobalScale = monitor_scale.x;
  } else {
    io.FontGlobalScale = monitor_scale.y;
  }
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  clear_color_ = ImVec4(0.f, 0.f, 0.f, 1.00f);
  style_colors_app();
}

App::~App() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
  glfwDestroyWindow(window_);
  glfwTerminate();
}

void App::run() {
  using namespace std::chrono_literals;

  start();

  while (!glfwWindowShouldClose(window_) && !close_requested()) {
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (ImGui::IsKeyPressed(ImGuiKey_Space) || ImGui::IsKeyPressed(ImGuiKey_P)) {
      invert_pause();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsKeyPressed(ImGuiKey_Q)) {
      request_close();
      pause();
    }

    update_model();

    if (!is_paused()) {
      static constexpr float PAD{10};
      const ImGuiViewport *viewport = ImGui::GetMainViewport();

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

      update_gui();

      ImGui::End();

      // Rendering
      ImGui::Render();

      int display_w, display_h;
      glfwGetFramebufferSize(window_, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glClearColor(clear_color_.x, clear_color_.y, clear_color_.z, clear_color_.w);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      update_frame();

      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      glfwSwapBuffers(window_);
    } else {
      // Tiny sleep so the CPU doesn't waste energy.
      std::this_thread::sleep_for(20ms);
    }

    VLOG(4) << "Frame rate: " << ImGui::GetIO().Framerate << " fps";
  }
}

GLuint App::initialize_shaders(std::filesystem::path vertex_file_path,
                               std::filesystem::path fragment_file_path) {
  // Create the shaders
  GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

  GLint result{GL_FALSE};
  int info_log_length{0};

  {
    const std::string vertex_shader_code{io::read_file(vertex_file_path)};
    VLOG(2) << "vertex shader:\n" << vertex_shader_code << "\n";
    const char *vertex_source_pointer = vertex_shader_code.c_str();
    glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, NULL);
    glCompileShader(vertex_shader_id);

    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
      glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
      if (info_log_length > 0) {
        LOG(INFO) << "result: " << result << ", info_log_length: " << info_log_length;
        std::vector<GLchar> error_message(info_log_length);
        glGetShaderInfoLog(vertex_shader_id, info_log_length, &info_log_length, &error_message[0]);
        LOG(FATAL) << "Could not compile vertex shader: " << &error_message[0];
      }
    }
  }

  {
    const std::string fragment_shader_code{io::read_file(fragment_file_path)};
    VLOG(2) << "fragment shader:\n" << fragment_shader_code << "\n";
    const char *fragment_source_pointer = fragment_shader_code.c_str();
    glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, NULL);
    glCompileShader(fragment_shader_id);

    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
      glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
      if (info_log_length > 0) {
        LOG(INFO) << "result: " << result << ", info_log_length: " << info_log_length;
        std::vector<GLchar> error_message(info_log_length);
        glGetShaderInfoLog(fragment_shader_id, info_log_length, &info_log_length,
                           &error_message[0]);
        LOG(FATAL) << "Could not compile fragment shader: " << &error_message[0];
      }
    }
  }

  // Link the program
  LOG(INFO) << "Linking shader program.";
  GLuint program_id = glCreateProgram();
  glAttachShader(program_id, vertex_shader_id);
  glAttachShader(program_id, fragment_shader_id);
  glLinkProgram(program_id);

  // Check the program
  glGetProgramiv(program_id, GL_LINK_STATUS, &result);
  if (result == GL_FALSE) {
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
      LOG(INFO) << "result: " << result << ", info_log_length: " << info_log_length;
      std::vector<GLchar> error_message(info_log_length);
      glGetShaderInfoLog(fragment_shader_id, info_log_length, &info_log_length, &error_message[0]);
      LOG(FATAL) << "Could not link shader program: " << &error_message[0];
    }
  }

  glDetachShader(program_id, vertex_shader_id);
  glDetachShader(program_id, fragment_shader_id);

  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  VLOG(3) << "program_id: " << program_id;
  return program_id;
}

}  // namespace ndyn::ui
