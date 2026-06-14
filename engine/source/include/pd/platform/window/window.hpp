#pragma once

namespace pd {
class IWindow {
 public:
  enum class Type : uint8_t {
    Win32,
    SDL3,
    Wayland,
    Android,
  };
  struct Config {
    std::string title = global::DefaultWindowTitle;
    uint32_t width = global::DefaultWindowWidth;
    uint32_t height = global::DefaultWindowHeight;
  };
  explicit IWindow() noexcept = default;
  virtual ~IWindow() = default;
  MOVABLE_ONLY(IWindow);

  // common opts
  virtual bool create() noexcept = 0;
  virtual void close() noexcept = 0;

  virtual void resize(uint32_t width, uint32_t height) noexcept = 0;
  virtual void setTitle(const std::string& title) noexcept = 0;

  // common getter
  [[nodiscard]] virtual void* nativeHandle() const noexcept { return nullptr; };
  [[nodiscard]] virtual bool isRunning() const noexcept { return false; };
  [[nodiscard]] virtual bool shouldClose() const noexcept { return true; };
  [[nodiscard]] virtual uint32_t windowHeight() const noexcept { return 0; }
  [[nodiscard]] virtual uint32_t windowWidth() const noexcept { return 0; }
  [[nodiscard]] virtual std::string windowTitle() const noexcept { return {}; }

  // backend sepecific
  [[nodiscard]] virtual std::vector<const char*> getVulkanInstanceExtensions() noexcept {
    return {};
  }
};
}  // namespace pd