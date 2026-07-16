#include "pd/platform/window/sdl3/window_sdl3.hpp"

#include "SDL3/sdl.h"
#ifdef BACKEND_VULKAN
#include "SDL3/SDL_vulkan.h"
#endif

namespace pd {
WindowSDL3::WindowSDL3(IWindow::Config config) noexcept
    : mConfig(std::move(config)) {}

WindowSDL3::~WindowSDL3() { closeImpl(); }

bool WindowSDL3::create() noexcept {
  if (!mClosed) {
    return true;
  }
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    LOG_ERROR("SDL init failed! {}", SDL_GetError());
    return false;
  }
  mWindow = SDL_CreateWindow(
      mConfig.title.c_str(), static_cast<int>(mConfig.width),
      static_cast<int>(mConfig.height),
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
  if (mWindow == nullptr) {
    LOG_ERROR("SDL create window failed! {}", SDL_GetError());
    return false;
  }

  mClosed = false;
  return true;
}

void WindowSDL3::close() noexcept { closeImpl(); }

void WindowSDL3::closeImpl() noexcept {
  if (mWindow == nullptr) {
    return;
  }
  SDL_DestroyWindow(mWindow);
  SDL_Quit();

  mClosed = true;
}

void WindowSDL3::resize(uint32_t width, uint32_t height) noexcept {
  // TODO(ryu613)
}

void WindowSDL3::setTitle(const std::string& title) noexcept {
  // TODO(ryu613)
}

void* WindowSDL3::nativeHandle() const noexcept {
#ifdef _WIN32
  return SDL_GetPointerProperty(SDL_GetWindowProperties(mWindow),
                                SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
#else
  PD_ASSERT_MSG(false, "WINDOW HANDLE ERROR! UNSUPPORTED PLATFORM");
#endif
}

std::vector<const char*> WindowSDL3::getVulkanInstanceExtensions() noexcept {
  uint32_t instanceExtensionsCount{0};
#ifdef BACKEND_VULKAN
  const char* const* instanceExtensions{
      SDL_Vulkan_GetInstanceExtensions(&instanceExtensionsCount)};
  return std::vector<const char*>(instanceExtensions,
                                  instanceExtensions + instanceExtensionsCount);
#else
  PD_ASSERT_MSG(false, "illegal operation!");
#endif
}

void WindowSDL3::processEvent() noexcept {
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
}  // namespace pd