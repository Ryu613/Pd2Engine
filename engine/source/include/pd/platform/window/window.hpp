#pragma once

namespace pd {
class IWindow {
 public:
  using Handle = void*;

  explicit IWindow() noexcept = default;
  virtual ~IWindow() = default;
  DELETE_COPY(IWindow);
  DEFAULT_MOVABLE(IWindow);

  // common opts
  virtual Result<IWindow::Handle> init(const WindowConfig& config) noexcept = 0;
  virtual bool createWindow() noexcept = 0;
  virtual void closeWindow() noexcept = 0;

  virtual void resize(uint32_t width, uint32_t height) noexcept = 0;
  virtual void setTitle(const std::string& title) noexcept = 0;
  virtual void processEvent() noexcept = 0;

  // common getter
  [[nodiscard]] virtual void* nativeHandle() const noexcept { return nullptr; };
  [[nodiscard]] virtual bool isRunning() const noexcept { return false; };
  [[nodiscard]] virtual bool shouldClose() const noexcept { return true; };
  [[nodiscard]] virtual uint32_t windowHeight() const noexcept { return 0; }
  [[nodiscard]] virtual uint32_t windowWidth() const noexcept { return 0; }
  [[nodiscard]] virtual std::string windowTitle() const noexcept { return {}; }

  // backend sepecific
  [[nodiscard]] virtual std::vector<cstr> getVulkanInstanceExtensions() noexcept { return {}; }
};
}  // namespace pd