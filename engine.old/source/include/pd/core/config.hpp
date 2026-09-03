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
  WindowConfig window{};
  std::string assetBaseDir = ".";
};
struct EngineConfig {
  std::string appName = "default";
  PlatformConfig platform{};
  bool enableDebug = global::enableDebug;
};
}  // namespace pd