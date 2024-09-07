cc_library(
    name = "interface",
    hdrs = glob(["*.h"]),
    includes = ["."],
)

cc_library(
    name = "opengl3_backend",
    srcs = [
        "backends/imgui_impl_opengl3.cpp",
    ],
    hdrs = [
        "backends/imgui_impl_opengl3.h",
        "backends/imgui_impl_opengl3_loader.h",
    ],
    strip_include_prefix = "backends/",
    deps = [
        ":interface",
    ],
)

cc_library(
    name = "glfw_backend",
    srcs = [
        "backends/imgui_impl_glfw.cpp",
    ],
    hdrs = [
        "backends/imgui_impl_glfw.h",
    ],
    strip_include_prefix = "backends/",
    deps = [
        ":interface",
        "@com_github_glfw//:glfw",
    ],
)

cc_library(
    name = "imgui",
    srcs = glob(["*.cpp"]),
    visibility = ["//visibility:public"],
    deps = [
        ":glfw_backend",
        ":interface",
        ":opengl3_backend",
    ],
)
