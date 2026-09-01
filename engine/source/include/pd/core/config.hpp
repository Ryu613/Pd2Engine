#pragma once

namespace pd {
struct WindowConfig {
  std::string title = global::DefaultWindowTitle;
  u32 width = global::DefaultWindowWidth;
  u32 height = global::DefaultWindowHeight;
  bool resizable = true;
  bool headless = false;
  WindowType type = WindowType::SDL3;
};
struct EngineConfig {
  std::string appName = global::DefaultWindowTitle;
  WindowConfig window{};
  bool enableDebug = global::enableDebug;
  std::string assetDir = ".";
};
}  // namespace pd