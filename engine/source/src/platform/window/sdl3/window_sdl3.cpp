#pragma once

#include "pd/platform/window/sdl3/window_sdl3.hpp"

#include "SDL3/sdl.h"

namespace pd {
WindowSDL3::WindowSDL3(Window::Config config) noexcept
    : mConfig(std::move(config)) {}

WindowSDL3::~WindowSDL3() noexcept { close(); }

bool WindowSDL3::create() noexcept {
  if (!mClosed) {
    return true;
  }
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    log::error("SDL init failed! {}", SDL_GetError());
    return false;
  }
  mWindow = SDL_CreateWindow(
      mConfig.title.c_str(), static_cast<int>(mConfig.width),
      static_cast<int>(mConfig.height),
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
  if (mWindow == nullptr) {
    log::error("SDL create window failed! {}", SDL_GetError());
    return false;
  }

  mClosed = false;
  return true;
}

void WindowSDL3::close() noexcept {
  if (mWindow == nullptr) {
    return;
  }
  SDL_DestroyWindow(mWindow);
  SDL_Quit();

  mClosed = true;
}

void WindowSDL3::resize(uint32_t width, uint32_t height) noexcept {
  // TODO(author)
}

void WindowSDL3::setTitle(const std::string& title) noexcept {
  // TODO(author)
}
}  // namespace pd