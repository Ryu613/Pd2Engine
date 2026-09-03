#pragma once

#include "pd/platform/window/window.hpp"

namespace pd {
// TODO(ryu613)
class WindowWin32 : public IWindow {
 public:
  Result<IWindow::Handle> init(const WindowConfig& config) noexcept override;
  bool createWindow() noexcept override;
  void closeWindow() noexcept override;

  void resize(uint32_t width, uint32_t height) noexcept override;
  void setTitle(const std::string& title) noexcept override;

  [[nodiscard]] bool isRunning() const noexcept override;
  [[nodiscard]] uint32_t windowHeight() const noexcept override;
  [[nodiscard]] uint32_t windowWidth() const noexcept override;
  [[nodiscard]] std::string windowTitle() const noexcept override;
};
}  // namespace pd