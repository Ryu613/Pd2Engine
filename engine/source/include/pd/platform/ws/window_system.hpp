#pragma once

namespace pd {
/**
 * @brief 窗口系统，编译期确定，非虚
 *
 */
class WindowSystem {
 public:
  WindowSystem();
  ~WindowSystem();
  DELETE_COPY_MOVE(WindowSystem);

  Result<void> init(const WindowConfig& config) noexcept;
  Result<void> destroy() noexcept;

  Result<void> createWindow() noexcept;
  void processEvents() noexcept;

  [[nodiscard]] void* nativeWindowHandle() const noexcept;
  bool shouldClose() const noexcept;

 private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};
}  // namespace pd