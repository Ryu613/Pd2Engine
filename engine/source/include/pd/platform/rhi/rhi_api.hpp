#pragma once

#include "pd/platform/rhi/rhi_handle.hpp"

namespace pd {
struct RhiTexture;
/**
 * @brief 代表渲染底层API的高层抽象，负责统一对不同API的调用差异，实现更方便的切换
 *
 */
class RhiApi {
 public:
  RhiApi() noexcept = default;
  virtual ~RhiApi() = default;

  NO_COPY_MOVE(RhiApi);

  virtual RhiHandle<RhiTexture> createTexture(uint32_t width, uint32_t height,
                                              uint32_t depth) noexcept = 0;

  virtual void updateTexture(RhiHandle<RhiTexture> handle, void* pPixelData) noexcept = 0;

  virtual void destroyTexture(RhiHandle<RhiTexture> handle) noexcept = 0;
};
}  // namespace pd