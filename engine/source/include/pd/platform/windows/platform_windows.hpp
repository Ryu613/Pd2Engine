#pragma once

#include "pd/platform/platform.hpp"

#include "SDL3/SDL.h"

namespace pd {
class PlatformWindows : public Platform {
 public:
  PlatformWindows() noexcept;
  ~PlatformWindows() noexcept;

  bool initialize(const std::string& appName, uint32_t width, uint32_t height,
                  bool enableDebug) noexcept override;

  void shutdown() noexcept override;

  void processEvents() noexcept override;

  bool shouldClose() noexcept override;

  RhiApi& getRhiApi() noexcept override;

  FileSystem& getFileSystem() noexcept override;

 private:
  bool mClosed = false;

  std::unique_ptr<FileSystem> mFileSystem = nullptr;

  SDL_Window* mWindow = nullptr;

  std::unique_ptr<RhiApi> mRhiApi;

  void* getNativeWindowHandle() noexcept;

  std::vector<const char*> getVulkanInstanceExts() noexcept;
};
}  // namespace pd