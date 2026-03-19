#pragma once

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>

#include "tools/cpp/runfiles/runfiles.h"

namespace ndyn {

/**
 * Structure to find resource files from the build outputs and read them at runtime without
 * hardcoding any absolute paths or assumptions about the layout of the build output.
 */
class ResourceLoader final {
 private:
  static std::once_flag init_flag_;
  static std::unique_ptr<ResourceLoader> instance_;

  std::unique_ptr<bazel::tools::cpp::runfiles::Runfiles> runfiles_;

  ResourceLoader(std::unique_ptr<bazel::tools::cpp::runfiles::Runfiles>&& runfiles)
      : runfiles_(std::move(runfiles)) {}

  ResourceLoader(bazel::tools::cpp::runfiles::Runfiles* runfiles)
      : runfiles_(std::move(runfiles)) {}

 public:
  ResourceLoader(const ResourceLoader&) = delete;
  ResourceLoader& operator=(const ResourceLoader&) = delete;

  std::filesystem::path resolve(std::string_view workspace_path) const;
  std::string load(std::string_view workspace_path) const;

  static void initialize(char* name);
  static ResourceLoader& instance();
};

}  // namespace ndyn
