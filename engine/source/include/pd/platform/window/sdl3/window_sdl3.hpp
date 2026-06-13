#pragma once

#include "pd/platform/window/window.hpp"

struct SDL_Window;
namespace pd {
class WindowSDL3 : public Window {
 public:
  explicit WindowSDL3(Window::Config config) noexcept;
  ~WindowSDL3() noexcept;
  MOVABLE_ONLY(WindowSDL3);

  bool create() noexcept override;
  void close() noexcept override;

  void resize(uint32_t width, uint32_t height) noexcept override;
  void setTitle(const std::string& title) noexcept override;

  [[nodiscard]] void* handle() const noexcept override { return mWindow; };
  [[nodiscard]] bool isRunning() const noexcept override { return !mClosed; }
  [[nodiscard]] bool shouldClose() const noexcept override { return mClosed; }
  [[nodiscard]] uint32_t windowHeight() const noexcept override { return mConfig.height; }
  [[nodiscard]] uint32_t windowWidth() const noexcept override { return mConfig.width; }
  [[nodiscard]] std::string windowTitle() const noexcept override {
    return mConfig.title;
  }

 private:
  friend class SDL3Helper;
  Config mConfig;
  bool mClosed = true;

  SDL_Window* mWindow = nullptr;
};
}  // namespace pd