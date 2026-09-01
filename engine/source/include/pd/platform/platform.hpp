#pragma once

#include "pd/platform/file/file_system.hpp"
#include "pd/platform/window/window.hpp"

namespace pd {
/**
 * @brief 平台层
 *
 */
class IPlatform {
 public:
  enum class BaseDir : u8 {
    AppDir,
    DataDir,
    AssetsDir,
  };
  struct Config {
    WindowConfig window;
  };

  IPlatform() noexcept = default;
  virtual ~IPlatform() = default;
  DELETE_COPY(IPlatform);
  DEFAULT_MOVABLE(IPlatform);

  [[nodiscard]] virtual IFileSystem* fileSystem() noexcept { return nullptr; }
  [[nodiscard]] virtual IWindow* window() noexcept { return nullptr; };

  //   virtual bool initialize(const std::string& appName, uint32_t width, uint32_t
  //   height,
  //                           bool enableDebug) noexcept = 0;

  //   virtual void shutdown() noexcept = 0;

  virtual void processEvents() noexcept {}

  //   virtual bool shouldClose() noexcept = 0;

  //   [[nodiscard]] virtual RhiApi& getRhiApi() noexcept = 0;
};

}  // namespace pd