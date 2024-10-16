DEFINES = select({
    "@platforms//os:linux": ["_GLFW_X11"],
    "@platforms//os:windows": ["_GLFW_WIN32"],
    "//conditions:default": [],
})

COMMON_HEADERS = glob(["include/GLFW/*.h"]) + [
    "src/internal.h",
    "src/mappings.h",
]

PLATFORM_HEADERS = select({
    "@platforms//os:linux": [
        "src/x11_platform.h",
        "src/xkb_unicode.h",
        "src/posix_time.h",
        "src/posix_thread.h",
        "src/glx_context.h",
        "src/egl_context.h",
        "src/osmesa_context.h",
        "src/linux_joystick.h",
    ],
    "@platforms//os:windows": [
        "src/win32_platform.h",
        "src/win32_joystick.h",
        "src/wgl_context.h",
        "src/egl_context.h",
        "src/osmesa_context.h",
    ],
    "//conditions:default": [],
})

COMMON_SRCS = [
    "src/context.c",
    "src/init.c",
    "src/input.c",
    "src/monitor.c",
    "src/vulkan.c",
    "src/window.c",
]

PLATFORM_SRCS = select({
    "@platforms//os:linux": [
        "src/x11_init.c",
        "src/x11_monitor.c",
        "src/x11_window.c",
        "src/xkb_unicode.c",
        "src/posix_time.c",
        "src/posix_thread.c",
        "src/glx_context.c",
        "src/egl_context.c",
        "src/osmesa_context.c",
        "src/linux_joystick.c",
    ],
    "@platforms//os:windows": [
        "src/win32_init.c",
        "src/win32_joystick.c",
        "src/win32_monitor.c",
        "src/win32_time.c",
        "src/win32_thread.c",
        "src/win32_window.c",
        "src/wgl_context.c",
        "src/egl_context.c",
        "src/osmesa_context.c",
    ],
    "//conditions:default": [],
})

cc_library(
    name = "glfw_headers",
    hdrs = glob(["include/GLFW/*.h"]),
    defines = DEFINES,
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "glfw",
    srcs = COMMON_SRCS + PLATFORM_SRCS + COMMON_HEADERS + PLATFORM_HEADERS,
    defines = DEFINES,
    linkopts = [
        "-lpthread",
        "-ldl",
        "-lGL",
        "-lX11",
    ],
    visibility = ["//visibility:public"],
    deps = [":glfw_headers"],
    alwayslink = True,
)
