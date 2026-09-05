#include "pd/platform/ws/window_system.hpp"

#include "sdl3/SDL.h"

namespace pd {
class WindowSystem::Impl {
 public:
  Result<void> init(const WindowConfig& config) noexcept {
    mConfig = config;
    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
      LOG_ERROR("SDL init failed! {}", SDL_GetError());
      return make_error<void>(ErrorCode::WindowInitFailed);
    }

    return {};
  }

  Result<void> destroy() noexcept {
    if (mWindow) {
      SDL_DestroyWindow(mWindow);
      mClosed = true;
    }
    return {};
  }

  Result<void> createWindow() noexcept {
    mWindow = SDL_CreateWindow(
        mConfig.title.c_str(), static_cast<int>(mConfig.width), static_cast<int>(mConfig.height),
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (mWindow == nullptr) {
      LOG_ERROR("SDL create window failed! {}", SDL_GetError());
      return make_error<void>(ErrorCode::WindowInitFailed);
    }
    mClosed = false;
    return {};
  }

  void* nativeWindowHandle() const noexcept {
#ifdef _WIN32
    return SDL_GetPointerProperty(SDL_GetWindowProperties(mWindow),
                                  SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
#else
    PD_ASSERT_MSG(false, "WINDOW HANDLE ERROR! UNSUPPORTED PLATFORM");
#endif
  }

  bool shouldClose() const noexcept { return mClosed; }

  void processEvents() noexcept {
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

 private:
  WindowConfig mConfig;
  SDL_Window* mWindow = nullptr;
  bool mClosed = true;
};

WindowSystem::WindowSystem()
    : mImpl(std::make_unique<WindowSystem::Impl>()) {}

WindowSystem::~WindowSystem() = default;

Result<void> WindowSystem::init(const WindowConfig& config) noexcept { return mImpl->init(config); }

Result<void> WindowSystem::destroy() noexcept { return mImpl->destroy(); }

Result<void> WindowSystem::createWindow() noexcept { return mImpl->createWindow(); }

void WindowSystem::processEvents() noexcept { mImpl->processEvents(); }

void* WindowSystem::nativeWindowHandle() const noexcept { return mImpl->nativeWindowHandle(); }

bool WindowSystem::shouldClose() const noexcept { return mImpl->shouldClose(); }
}  // namespace pd