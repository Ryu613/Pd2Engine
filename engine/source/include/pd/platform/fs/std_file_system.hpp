#pragma once

#include "pd/platform/fs/file_system.hpp"

namespace pd {
class StdFileSystem final : public IFileSystem {
 public:
  StdFileSystem() = default;
  ~StdFileSystem() = default;

  DELETE_COPY(StdFileSystem);
  DEFAULT_MOVABLE(StdFileSystem);

  FileStat statOfFile(const Path& path) noexcept override;
  bool isFile(const Path& path) noexcept override;
  bool isDirectory(const Path& path) noexcept override;
  bool exists(const Path& path) noexcept override;
  bool createDirectory(const Path& path) noexcept override;
  std::vector<uint8_t> readChunk(const Path& path, size_t offset, size_t count) noexcept override;
  bool writeFile(const Path& path, const std::vector<uint8_t>& data) noexcept override;
  bool remove(const Path& path) noexcept override;
};
}  // namespace pd