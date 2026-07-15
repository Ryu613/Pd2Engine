#pragma once

#include "pd/platform/window/window.hpp"

struct SDL_Window;
namespace pd {
class WindowSDL3 : public IWindow {
 public:
  explicit WindowSDL3(IWindow::Config config) noexcept;
  ~WindowSDL3() override;
  DELETE_COPY(WindowSDL3);
  DEFAULT_MOVABLE(WindowSDL3);

  bool create() noexcept override;
  void close() noexcept override;

  void resize(uint32_t width, uint32_t height) noexcept override;
  void setTitle(const std::string& title) noexcept override;
  void processEvent() noexcept override;

  [[nodiscard]] void* nativeHandle() const noexcept override;
  [[nodiscard]] bool isRunning() const noexcept override { return !mClosed; }
  [[nodiscard]] bool shouldClose() const noexcept override { return mClosed; }
  [[nodiscard]] uint32_t windowHeight() const noexcept override { return mConfig.height; }
  [[nodiscard]] uint32_t windowWidth() const noexcept override { return mConfig.width; }
  [[nodiscard]] std::string windowTitle() const noexcept override { return mConfig.title; }

  [[nodiscard]] std::vector<const char*> getVulkanInstanceExtensions() noexcept override;

 private:
  Config mConfig;
  bool mClosed = true;

  SDL_Window* mWindow = nullptr;

  void closeImpl() noexcept;
};
}  // namespace pd