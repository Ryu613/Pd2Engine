#pragma once

#include "pd/backend/hw_enums.hpp"
#include "pd/backend/hw_resource.hpp"

namespace pd {
struct TextureOptions {
  std::string debugLabel;
  TextureType type = TextureType::TextureType2D;
  TextureFormat format = TextureFormat::RGBA8_Unorm;
  Extent extent = {
      .width = 0,
      .height = 0,
      .depth = 0,
  };
  uint32_t mipLevel = 1;
  uint32_t arrayLeyers = 1;
  TextureUsage usage = TextureUsage::DefaultUsage;
  uint32_t samples = 1;
  TextureLayout initialLayout = TextureLayout::Undefined;
  bool isProtected = false;
};
/**
 * @brief gpu层texture表示,纯结构体，只用于记录跨backend的基本信息
 *
 */
struct HwTexture : public HwResource {
  explicit HwTexture(TextureOptions options) noexcept
      : HwResource(),
        options(std::move(options)) {}
  TextureOptions options;
};
}  // namespace pd