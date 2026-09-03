#include "pd/platform/fs/std_file_system.hpp"

#include <fstream>
#include <system_error>

namespace pd {

FileStat StdFileSystem::statOfFile(const Path& path) noexcept {
  std::error_code ec;

  auto fsStat = std::filesystem::status(path, ec);
  if (ec) {
    return FileStat{
        .isFile = false,
        .isDirectory = false,
        .size = 0,
    };
  }

  auto size = std::filesystem::file_size(path, ec);
  if (ec) {
    size = 0;
  }

  return FileStat{
      .isFile = fsStat.type() == std::filesystem::file_type::regular,
      .isDirectory = fsStat.type() == std::filesystem::file_type::directory,
      .size = size,
  };
}

bool StdFileSystem::isFile(const Path& path) noexcept {
  auto stat = statOfFile(path);
  return stat.isFile;
}

bool StdFileSystem::isDirectory(const Path& path) noexcept {
  auto stat = statOfFile(path);
  return stat.isDirectory;
}

bool StdFileSystem::exists(const Path& path) noexcept {
  auto stat = statOfFile(path);
  return stat.isDirectory || stat.isFile;
}

bool StdFileSystem::createDirectory(const Path& path) noexcept {
  std::error_code ec;
  std::filesystem::create_directories(path, ec);
  if (ec) {
    LOG_ERROR("failed to create directory: {}", path.string());
    return false;
  }
  return true;
}

std::vector<uint8_t> StdFileSystem::readChunk(const Path& path, size_t offset,
                                              size_t count) noexcept {
  std::ifstream file{path, std::ios::binary | std::ios::ate};

  if (!file.is_open()) {
    LOG_ERROR("failed to open file: {}", path.string());
    return {};
  }

  auto size = statOfFile(path).size;

  if (offset + count > size) {
    return {};
  }
  // read file binaries
  file.seekg(offset, std::ios::beg);
  std::vector<uint8_t> data(count);
  file.read(reinterpret_cast<char*>(data.data()), count);
  return data;
}

bool StdFileSystem::writeFile(const Path& path, const std::vector<uint8_t>& data) noexcept {
  auto parentPath = path.parent_path();
  if (!std::filesystem::exists(parentPath)) {
    createDirectory(parentPath);
  }

  std::ofstream file{path, std::ios::binary | std::ios::trunc};

  if (!file.is_open()) {
    LOG_ERROR("failed to open file to write: {}", path.string());
    return false;
  }

  file.write(reinterpret_cast<const char*>(data.data()), data.size());

  return true;
}

bool StdFileSystem::remove(const Path& path) noexcept {
  std::error_code ec;

  std::filesystem::remove_all(path, ec);

  if (ec) {
    LOG_ERROR("failed to remove file: {}", path.string());
    return false;
  }

  return true;
}
}  // namespace pd
