load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def load_source_dependencies():
    if not native.existing_rule("com_google_googletest"):
        http_archive(
            name = "com_google_googletest",
            sha256 = "3c2a0999fb9f85423d17e3000b339fd28501433f70c2e9941c8e784ac8624a10",
            strip_prefix = "googletest-cead3d57c93ff8c4e5c1bbae57a5c0b0b0f6e168",
            urls = ["https://github.com/google/googletest/archive/cead3d57c93ff8c4e5c1bbae57a5c0b0b0f6e168.zip"],
        )

    if not native.existing_rule("com_github_google_benchmark"):
        http_archive(
            name = "com_github_google_benchmark",
            sha256 = "2a778d821997df7d8646c9c59b8edb9a573a6e04c534c01892a40aa524a7b68c",
            strip_prefix = "benchmark-bf585a2789e30585b4e3ce6baf11ef2750b54677",
            urls = ["https://github.com/google/benchmark/archive/bf585a2789e30585b4e3ce6baf11ef2750b54677.zip"],
        )

    if not native.existing_rule("com_github_gflags_gflags"):
        http_archive(
            name = "com_github_gflags_gflags",
            sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
            strip_prefix = "gflags-2.2.2",
            urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
        )

    if not native.existing_rule("com_github_google_glog"):
        http_archive(
            name = "com_github_google_glog",
            sha256 = "21bc744fb7f2fa701ee8db339ded7dce4f975d0d55837a97be7d46e8382dea5a",
            urls = ["https://github.com/google/glog/archive/v0.5.0.zip"],
            strip_prefix = "glog-0.5.0",
        )

    if not native.existing_rule("zlib"):
        http_archive(
            name = "zlib",
            sha256 = "1525952a0a567581792613a9723333d7f8cc20b87a81f920fb8bc7e3f2251428",
            strip_prefix = "zlib-1.2.13",
            urls = [
                "https://github.com/madler/zlib/archive/refs/tags/v1.2.13.tar.gz",
            ],
            build_file = "//third_party/zlib:zlib.BUILD",
        )

    if not native.existing_rule("peglib"):
        http_archive(
            name = "peglib",
            sha256 = "f57aa0f14372cbb772af29e3a4549a8033ea07eb25c39949cba6178e0e2ba9cc",
            strip_prefix = "cpp-peglib-1.9.1",
            urls = [
                "https://github.com/yhirose/cpp-peglib/archive/refs/tags/v1.9.1.tar.gz",
            ],
            build_file = "//third_party/peglib:peglib.BUILD",
        )
