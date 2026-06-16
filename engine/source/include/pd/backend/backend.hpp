#pragma once

namespace pd {
class IWindow;
class IBackend {
 public:
  struct Config {
    IWindow* pWindow = nullptr;
    bool enableDebug = true;
  };
  IBackend() noexcept = default;
  virtual ~IBackend() = default;
  MOVABLE_ONLY(IBackend);
};
}  // namespace pd