#pragma once

#include "pd/platform/platform.hpp"
#include "pd/platform/window/window.hpp"

namespace pd {
/**
 * @brief win32平台实现
 *
 * TODO:
 */
class PlatformWin32 : public IPlatform {
 public:
  explicit PlatformWin32(IPlatform::Config config) noexcept;
  ~PlatformWin32();
  MOVABLE_ONLY(PlatformWin32);

  IFileSystem* fileSystem() noexcept override { return mFileSystem.get(); }
  IWindow* window() noexcept override { return mWindow.get(); }

  //   bool initialize(const std::string& appName, uint32_t width, uint32_t height,
  //                   bool enableDebug) noexcept override;

  //   void shutdown() noexcept override;

  //   void processEvents() noexcept override;

  //   bool shouldClose() noexcept override;

  //   RhiApi& getRhiApi() noexcept override;

 private:
  IPlatform::Config mConfig;

  std::unique_ptr<IWindow> mWindow;
  std::unique_ptr<IFileSystem> mFileSystem;

  //   std::unique_ptr<RhiApi> mRhiApi;

  //   void* getNativeWindowHandle() noexcept;

  //   std::vector<const char*> getVulkanInstanceExts() noexcept;
};
}  // namespace pd