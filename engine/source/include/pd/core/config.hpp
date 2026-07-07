#pragma once

namespace pd {
struct EngineConfig {
  std::string appName = global::DefaultWindowTitle;
  u32 windowWidth = global::DefaultWindowWidth;
  u32 windowHeight = global::DefaultWindowHeight;
  bool enableDebug = global::enableDebug;
};
}  // namespace pd