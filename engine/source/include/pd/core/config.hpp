#pragma once

namespace pd {
struct EngineConfig {
  std::string appName = global::DefaultWindowTitle;
  uint32_t windowWidth = global::DefaultWindowWidth;
  uint32_t windowHeight = global::DefaultWindowHeight;
  bool enableDebug = global::enableDebug;
};
}  // namespace pd