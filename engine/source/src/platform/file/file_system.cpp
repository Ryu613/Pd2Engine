#include "pd/platform/file/file_system.hpp"

namespace pd {

void IFileSystem::writeFile(const Path& path, const std::string& data) noexcept {
  writeFile(path, std::vector<uint8_t>(data.begin(), data.end()));
}

std::string IFileSystem::readFileString(const Path& path) noexcept {
  auto binary = readFileBinary(path);
  return {binary.begin(), binary.end()};
}

std::vector<uint8_t> IFileSystem::readFileBinary(const Path& path) noexcept {
  auto stat = statOfFile(path);
  return readChunk(path, 0, stat.size);
}
}  // namespace pd