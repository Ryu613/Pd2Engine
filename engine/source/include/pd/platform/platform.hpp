#pragma once

#include "pd/platform/file/file_system.hpp"
#include "pd/platform/rhi/rhi_api.hpp"

namespace pd {
/**
 * @brief 平台层，统一处理操作系统及底层库调用差异
 *
 */
class Platform {
 public:
  Platform() noexcept = default;
  virtual ~Platform() = default;

  Platform(const Platform&) = delete;
  Platform& operator=(const Platform&) = delete;
  Platform(Platform&&) noexcept = delete;
  Platform& operator=(Platform&&) noexcept = delete;

  virtual bool initialize(const std::string& appName, uint32_t width, uint32_t height,
                          bool enableDebug) noexcept = 0;

  virtual void shutdown() noexcept = 0;

  virtual void processEvents() noexcept = 0;

  virtual bool shouldClose() noexcept = 0;

  [[nodiscard]] virtual RhiApi& getRhiApi() noexcept = 0;

  [[nodiscard]] virtual FileSystem& getFileSystem() noexcept = 0;
};

}  // namespace pd