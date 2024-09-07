cc_library(
    name = "interface",
    hdrs = glob(["*.h"]),
    includes = ["."],
)

cc_library(
    name = "implot",
    srcs = glob(
        ["*.cpp"],
    ),
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
        "@com_github_ocornut_imgui//:imgui",
    ],
)
