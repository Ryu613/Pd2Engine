#pragma once

#include "pd/platform/file/file_system.hpp"

namespace pd {
class StdFileSystem final : public FileSystem {
 public:
  StdFileSystem() = default;
  ~StdFileSystem() = default;

  MOVABLE_ONLY(StdFileSystem);

  FileStat statOfFile(const Path& path) noexcept override;
  bool isFile(const Path& path) noexcept override;
  bool isDirectory(const Path& path) noexcept override;
  bool exists(const Path& path) noexcept override;
  bool createDirectory(const Path& path) noexcept override;
  std::vector<uint8_t> readChunk(const Path& path, size_t offset,
                                 size_t count) noexcept override;
  bool writeFile(const Path& path, const std::vector<uint8_t>& data) noexcept override;
  bool remove(const Path& path) noexcept override;
};
}  // namespace pd