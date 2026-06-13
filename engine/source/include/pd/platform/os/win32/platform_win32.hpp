#pragma once

#include "pd/platform/platform.hpp"
#include "pd/platform/window/window.hpp"

namespace pd {
/**
 * @brief win32平台实现
 *
 * TODO:
 */
class PlatformWin32 : public Platform {
 public:
  explicit PlatformWin32(Platform::Config config) noexcept;
  ~PlatformWin32() noexcept;
  MOVABLE_ONLY(PlatformWin32);

  FileSystem* fileSystem() noexcept override { return mFileSystem.get(); }
  Window* window() noexcept override { return mWindow.get(); }

  //   bool initialize(const std::string& appName, uint32_t width, uint32_t height,
  //                   bool enableDebug) noexcept override;

  //   void shutdown() noexcept override;

  //   void processEvents() noexcept override;

  //   bool shouldClose() noexcept override;

  //   RhiApi& getRhiApi() noexcept override;

 private:
  Platform::Config mConfig;

  std::unique_ptr<Window> mWindow;
  std::unique_ptr<FileSystem> mFileSystem;

  //   std::unique_ptr<RhiApi> mRhiApi;

  //   void* getNativeWindowHandle() noexcept;

  //   std::vector<const char*> getVulkanInstanceExts() noexcept;
};
}  // namespace pd