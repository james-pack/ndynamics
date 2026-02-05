#include "io/utils.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include "glog/logging.h"

namespace ndyn::io {

std::string read_file(std::filesystem::path path) {
  std::ifstream file{path, std::ios::binary | std::ios::in | std::ios_base::ate};
  if (!file) {
    LOG(WARNING) << "Could not open file for reading. File: '" << path << "'";
    throw std::runtime_error("Could not open file for reading.");
  }

  const std::streamsize file_size{file.tellg()};
  if (file_size < 0) {
    LOG(WARNING) << "Could not determine file size. File: '" << path << "'";
    throw std::runtime_error("Could not determine file size.");
  }

  std::string contents(file_size, 0);

  file.seekg(0);

  if (!file.read(contents.data(), file_size)) {
    LOG(WARNING) << "Could not read file. File: '" << path << "'";
    throw std::runtime_error("Could not read file.");
  }

  return contents;
}

}  // namespace ndyn::io
