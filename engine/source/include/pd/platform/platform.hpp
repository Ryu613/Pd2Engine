#pragma once

#include "pd/platform/file/file_system.hpp"
#include "pd/platform/window/window.hpp"

namespace pd {
/**
 * @brief 平台层，统一处理操作系统及底层库调用差异
 *
 */
class IPlatform {
 public:
  enum class BaseDir : uint8_t {
    AppDir,
    DataDir,
    AssetsDir,
  };
  struct Config {
    IWindow::Config window;
  };

  IPlatform() noexcept = default;
  virtual ~IPlatform() = default;
  MOVABLE_ONLY(IPlatform);

  [[nodiscard]] virtual IFileSystem* fileSystem() noexcept { return nullptr; }
  [[nodiscard]] virtual IWindow* window() noexcept { return nullptr; };

  //   virtual bool initialize(const std::string& appName, uint32_t width, uint32_t
  //   height,
  //                           bool enableDebug) noexcept = 0;

  //   virtual void shutdown() noexcept = 0;

  //   virtual void processEvents() noexcept = 0;

  //   virtual bool shouldClose() noexcept = 0;

  //   [[nodiscard]] virtual RhiApi& getRhiApi() noexcept = 0;
};

}  // namespace pd