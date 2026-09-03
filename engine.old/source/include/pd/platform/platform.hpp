#pragma once

#include "pd/platform/file/file_system.hpp"
#include "pd/platform/window/window.hpp"

namespace pd {
/**
 * @brief 平台层
 *
 */
class IPlatform {
 public:
  IPlatform(const PlatformConfig& config);
  ~IPlatform() = default;
  DELETE_COPY_MOVE(IPlatform);

  void init() noexcept;
  void shutdown() noexcept;

  [[nodiscard]] IFileSystem* fileSystem() noexcept;
  [[nodiscard]] IWindow* window() noexcept;

  void processEvents() noexcept;
};

}  // namespace pd