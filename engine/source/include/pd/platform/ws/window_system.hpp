#pragma once

namespace pd {
class WindowSystem {
 public:
  WindowSystem() = default;
  ~WindowSystem() = default;
  DELETE_COPY_MOVE(WindowSystem);

  Result<void> init(const WindowConfig& config) noexcept;
  Result<void> destroy() noexcept;

  Result<void> createWindow() noexcept;
};
}  // namespace pd