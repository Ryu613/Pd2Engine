#include "pd/platform/windows/platform_windows.hpp"

#include "pd/platform/file/std_file_system.hpp"
#include "pd/platform/rhi/vulkan/rhi_vulkan.hpp"

#include "SDL3/SDL_vulkan.h"

namespace pd {

PlatformWindows::PlatformWindows() noexcept {}
PlatformWindows ::~PlatformWindows() noexcept { shutdown(); }
bool PlatformWindows::initialize(const std::string& appName, uint32_t width,
                                 uint32_t height, bool enableDebug) noexcept {
  log::info("initializing file system...");
  mFileSystem = std::make_unique<StdFileSystem>();
  log::info("initializing WSI...");
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    log::error("SDL init failed! {}", SDL_GetError());
    return false;
  }
  mWindow = SDL_CreateWindow(
      appName.c_str(), static_cast<int>(width), static_cast<int>(height),
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
  if (mWindow == nullptr) {
    log::error("SDL create window failed! {}", SDL_GetError());
    return false;
  }
  log::info("initializing backend driver...");
  VulkanConfig vkConfig{
      .appName = appName,
      .enableValidationLayer = enableDebug,
      .windowWidth = width,
      .windowHeight = height,
      .nativeWindowHandle = getNativeWindowHandle(),
      .requiredInstanceExtensions = getVulkanInstanceExts(),
  };
  mRhiApi = std::make_unique<RhiVulkan>(vkConfig);

  return true;
}
void PlatformWindows::shutdown() noexcept {
  log::info("destroying Driver...");
  mRhiApi.reset();

  log::info("destroying SDL3...");
  if (mWindow == nullptr) {
    return;
  }
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
}

void PlatformWindows::processEvents() noexcept {
  for (SDL_Event event; SDL_PollEvent(&event);) {
    // quit window
    if (event.type == SDL_EVENT_QUIT) {
      mClosed = true;
      break;
    }
    if (event.type == SDL_EVENT_KEY_DOWN) {
      // 按下esc退出
      if (event.key.key == SDLK_ESCAPE) {
        mClosed = true;
        break;
      }
    }
  }
}

bool PlatformWindows::shouldClose() noexcept { return mClosed; }

RhiApi& PlatformWindows::getRhiApi() noexcept { return *mRhiApi; }

FileSystem& PlatformWindows::getFileSystem() noexcept { return *mFileSystem; }

void* PlatformWindows::getNativeWindowHandle() noexcept {
  return SDL_GetPointerProperty(SDL_GetWindowProperties(mWindow),
                                SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
}

std::vector<const char*> PlatformWindows::getVulkanInstanceExts() noexcept {
  uint32_t instanceExtensionsCount{0};
  const char* const* instanceExtensions{
      SDL_Vulkan_GetInstanceExtensions(&instanceExtensionsCount)};
  return std::vector<const char*>(instanceExtensions,
                                  instanceExtensions + instanceExtensionsCount);
}
}  // namespace pd