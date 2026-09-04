#pragma once

namespace pd {
struct WindowConfig {
  std::string title = "PDEngine";
  u32 width = 1024;
  u32 height = 768;
  bool resizable = true;
  bool headless = false;
  WindowType type = WindowType::SDL3;
};
struct PlatformConfig {
  std::string assetBaseDir = ".";
  WindowConfig window{};
};
struct BackendConfig {
  void* pWindow = nullptr;
};
struct EngineConfig {
  std::string appName = "default";
  bool enableDebug = global::enableDebug;
  PlatformConfig platform{};
};
}  // namespace pd