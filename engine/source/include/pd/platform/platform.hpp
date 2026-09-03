#pragma once

#include "pd/platform/fs/file_system.hpp"
#include "pd/platform/ws/window_system.hpp"

namespace pd {
/**
 * @brief 平台层, 编译期确定，非虚
 *
 */
class Platform {
 public:
  Platform(const PlatformConfig& config);
  ~Platform() = default;
  DELETE_COPY_MOVE(Platform);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

  [[nodiscard]] IFileSystem& fileSystem() noexcept { return *mFileSystem; }
  [[nodiscard]] WindowSystem& windowSystem() noexcept { return mWindowSystem; }

 private:
  PlatformConfig mConfig;
  std::unique_ptr<IFileSystem> mFileSystem;
  WindowSystem mWindowSystem;
};

}  // namespace pd