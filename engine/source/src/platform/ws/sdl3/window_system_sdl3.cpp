#include "pd/platform/ws/window_system.hpp"

#include "sdl3/SDL.h"

namespace pd {
class WindowSystem::Impl {
 public:
  Impl() = default;
  ~Impl() = default;

  Result<void> init(const WindowConfig& config) noexcept {
    mConfig = config;
    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
      LOG_ERROR("SDL init failed! {}", SDL_GetError());
      return make_error<void>(ErrorCode::WindowInitFailed);
    }

    return {};
  }

  Result<void> destroy() noexcept { return {}; }

  Result<void> createWindow() noexcept { return {}; }

 private:
  WindowConfig mConfig;
};


WindowSystem::WindowSystem()
    : mImpl(std::make_unique<WindowSystem::Impl>()) {}

WindowSystem::~WindowSystem() = default;

Result<void> WindowSystem::init(const WindowConfig& config) noexcept {
  if (auto res = mImpl->init(config); !res) {
    return res;
  }
  return {};
}
Result<void> WindowSystem::destroy() noexcept {
  if (auto res = mImpl->destroy(); !res) {
    return res;
  }
  return {};
}

Result<void> WindowSystem::createWindow() noexcept {
  if (auto res = mImpl->createWindow(); !res) {
    return res;
  }
  return {};
}
}  // namespace pd