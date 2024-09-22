#include "io/utils.h"

#include <filesystem>
#include <fstream>
#include <string>

namespace ndyn::io {

std::string read_file(std::filesystem::path path) {
  std::string contents{};
  static constexpr size_t BUFFER_SIZE{1024};
  char buffer[BUFFER_SIZE];
  std::ifstream file{path, std::ios::binary | std::ios_base::in};

  size_t bytes_read{BUFFER_SIZE};
  while (bytes_read == BUFFER_SIZE) {
    file.read(buffer, BUFFER_SIZE);
    bytes_read = file.gcount();
    contents.append(buffer);
  }

  return contents;
}

}  // namespace ndyn::io
