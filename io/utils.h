#pragma once

#include <filesystem>
#include <string>

namespace ndyn::io {

std::string read_file(std::filesystem::path path);

}  // namespace ndyn::io
