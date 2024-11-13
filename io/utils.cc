#include "io/utils.h"

#include <filesystem>
#include <fstream>
#include <string>

namespace ndyn::io {

std::string read_file(std::filesystem::path path) {
  using std::to_string;
  std::string contents{};

  static constexpr size_t BUFFER_SIZE{1024};
  char buffer[BUFFER_SIZE];
  std::ifstream file{path, std::ios::binary | std::ios_base::in};

  size_t bytes_read{BUFFER_SIZE - 1};
  while (bytes_read == BUFFER_SIZE - 1) {
    file.read(buffer, BUFFER_SIZE - 1);
    bytes_read = file.gcount();
    // The file.read() call doesn't null-terminate its output in binary mode.
    buffer[bytes_read] = '\0';
    contents.append(buffer);
  }

  return contents;
}

}  // namespace ndyn::io
