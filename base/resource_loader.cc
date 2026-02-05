#include "base/resource_loader.h"

#include <filesystem>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>

#include "io/utils.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace ndyn {

static constexpr char WORKSPACE_NAME[] = "com_github_james-pack_ndynamics";

std::once_flag ResourceLoader::init_flag_;
std::unique_ptr<ResourceLoader> ResourceLoader::instance_;

void ResourceLoader::initialize(char* name) {
  std::call_once(init_flag_, [&]() {
    std::string error;
    auto runfiles = bazel::tools::cpp::runfiles::Runfiles::Create(name, &error);

    if (!runfiles) {
      throw std::runtime_error("Runfiles init failed: " + error);
    }

    instance_.reset(new ResourceLoader(std::move(runfiles)));
  });
}

ResourceLoader& ResourceLoader::instance() {
  if (!instance_) {
    throw std::logic_error("ResourceLoader accessed before initialized");
  }
  return *instance_;
}

std::filesystem::path ResourceLoader::resolve(std::string_view workspace_path) const {
  std::string path{workspace_path};
  if (path.find(WORKSPACE_NAME) != 0) {
    path = std::string(WORKSPACE_NAME) + "/" + path;
  }
  return runfiles_->Rlocation(path);
}

std::string ResourceLoader::load(std::string_view workspace_path) const {
  auto path{resolve(workspace_path)};
  return io::read_file(path);
}

}  // namespace ndyn
