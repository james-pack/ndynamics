cc_library(
    name = "units",
    hdrs = [
        "include/units.h",
    ],
    strip_include_prefix = "include/",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "units_test",
    testonly = True,
    srcs = [
        "unitTests/main.cpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":units",
        "@com_google_googletest//:gtest",
    ],
)
